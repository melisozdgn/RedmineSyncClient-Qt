#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "../Utils/ProgressBar.h"
#include "ConflictDialog.h"
#include "DescriptionDialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), currentRow(-1)
{
    ui->setupUi(this);
    QString style = "QTableView::item:selected { background-color: #F0F8FF; color: black; }"
                    "QTableView::item:hover { background-color: #F0F8FF; color: black; }";
    ui->tableView->setStyleSheet(style);
    ui->detailView->setStyleSheet(style);
    controller = new TaskController(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(controller->getModel());
    proxyModel->setFilterKeyColumn(-1);
    ui->tableView->setModel(proxyModel);
    ui->tableView->setItemDelegate(new ProgressBar(ui->tableView));
    detailModel = new QStandardItemModel(this);
    detailModel->setHorizontalHeaderLabels({"Property", "Value"});
    ui->detailView->setModel(detailModel);
    ui->detailView->horizontalHeader()->setStretchLastSection(true);
    ui->detailView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ganttView = new GanttView(this);
    ui->ganttLayout->addWidget(ganttView);
    connect(ui->btnSync, &QPushButton::clicked, this, &MainWindow::onSyncClicked);
    connect(ui->btnSave, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &MainWindow::onFilterChanged);
    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::onTableClicked);
    connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::onTableDoubleClicked);
    connect(ui->detailView, &QTableView::doubleClicked, this, &MainWindow::onDetailDoubleClicked);
    connect(controller, &TaskController::dataUpdated, this, &MainWindow::updateGantt);
    connect(controller, &TaskController::conflictFound, this, &MainWindow::onConflictDetected);
    controller->init();
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSyncClicked()
{
    controller->sync();
}

void MainWindow::onSaveClicked() {}

void MainWindow::onTableClicked(const QModelIndex &index)
{
    currentRow = proxyModel->mapToSource(index).row();
    Task t = controller->getTask(currentRow);
    updateDetailView(t);
}

void MainWindow::onTableDoubleClicked(const QModelIndex &index)
{
    currentRow = proxyModel->mapToSource(index).row();
    Task t = controller->getTask(currentRow);
    DescriptionDialog dlg(t.description, this);

    if(dlg.exec() == QDialog::Accepted)
    {
        t.description = dlg.getDescription();
        controller->update(currentRow, t);
        updateDetailView(t);
    }
}
void MainWindow::onDetailDoubleClicked(const QModelIndex &index)
{
    if(currentRow < 0) return;
    QString propName = detailModel->item(index.row(), 0)->text();
    if(propName == "Description") {
        Task t = controller->getTask(currentRow);
        DescriptionDialog dlg(t.description, this);
        if(dlg.exec() == QDialog::Accepted) {
            t.description = dlg.getDescription();
            controller->update(currentRow, t);
            updateDetailView(t);
        }
    }
}

void MainWindow::onFilterChanged(const QString &text)
{
    proxyModel->setFilterFixedString(text);
}
void MainWindow::updateDetailView(const Task& t)
{
    detailModel->removeRows(0, detailModel->rowCount());
    auto addRow = [&](QString p, QString v){
        detailModel->appendRow({new QStandardItem(p), new QStandardItem(v)});
    };

    addRow("Subject", t.subject);
    addRow("Status", t.status);
    addRow("Description", t.description);
    addRow("Priority", t.priority);
    addRow("Start Date", t.startDate);
    addRow("Due Date", t.dueDate);

    for(auto it = t.customFields.begin(); it != t.customFields.end(); ++it)
    {
        addRow(it.key(), it.value());
    }
}

void MainWindow::updateGantt()
{
    ganttView->updateTasks(controller->getTasks());
}
void MainWindow::onConflictDetected(int row, Task serverTask)
{
    Task localTask = controller->getTask(row);
    ConflictDialog dlg(localTask, serverTask, this);

    if(dlg.exec() == QDialog::Accepted)
    {
        if(dlg.useLocal())
        {
            controller->keepLocalAndPush(row);
        } else {
            controller->resolveConflict(row, serverTask);
        }
    }
}
