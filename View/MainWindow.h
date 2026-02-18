#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include "../Controller/TaskController.h"
#include "../Model/Task.h"
#include "GanttView.h"

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSyncClicked();
    void onSaveClicked();
    void onTableClicked(const QModelIndex &index);
    void onTableDoubleClicked(const QModelIndex &index);
    void onDetailDoubleClicked(const QModelIndex &index);
    void onFilterChanged(const QString &text);
    void onConflictDetected(int row, Task serverTask);
    void updateGantt();

private:
    Ui::MainWindow *ui;
    TaskController *controller;
    QSortFilterProxyModel *proxyModel;
    QStandardItemModel *detailModel;
    GanttView *ganttView;
    int currentRow;
    void updateDetailView(const Task& t);
};
#endif
