#include "ConflictDialog.h"
#include "ui_ConflictDialog.h"
#include <QStringList>
#include <QRegularExpression>
#include <QVector>
#include <algorithm>

ConflictDialog::ConflictDialog(const Task& local, const Task& server, QWidget *parent) :

    QDialog(parent), ui(new Ui::ConflictDialog), resultLocal(false){
    ui->setupUi(this);
    buildComparison(local, server);
    connect(ui->btnLocal, &QPushButton::clicked, this, &ConflictDialog::onLocalClicked);
    connect(ui->btnServer, &QPushButton::clicked, this, &ConflictDialog::onServerClicked);
}

ConflictDialog::~ConflictDialog() { delete ui; }


QStringList tokenize(const QString& text)
{
    QString temp = text;

    temp.replace(QRegularExpression("([\\.,;!\\?\\(\\)\\[\\]\\{\\}\":])"), " \\1 ");
    return temp.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
}

QString ConflictDialog::getDiffHtml(const QString& mainText, const QString& otherText, const QString& color, const QString& bgColor)
{
    QStringList w1 = tokenize(mainText);
    QStringList w2 = tokenize(otherText);

    int m = w1.size(), n = w2.size();

    QVector<QVector<int>> dp(m + 1, QVector<int>(n + 1, 0));

    for (int i = 1; i <= m; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            if (w1[i-1] == w2[j-1]) dp[i][j] = dp[i-1][j-1] + 1;
            else dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
        }
    }

    QStringList res;
    int i = m, j = n;

    while (i > 0 && j > 0)
    {
        if (w1[i-1] == w2[j-1]) {
            res.prepend("<span style='color:black'>" + w1[i-1] + "</span>");
            i--; j--;
        } else if (dp[i-1][j] > dp[i][j-1]) {
            res.prepend("<span style='color:" + color + "; background-color:" + bgColor + "; font-weight:bold'>" + w1[i-1] + "</span>");
            i--;
        } else {
            j--;
        }
    }

    while (i > 0)
    {
        res.prepend("<span style='color:" + color + "; background-color:" + bgColor + "; font-weight:bold'>" + w1[i-1] + "</span>");
        i--;
    }

    return res.join(" ");
}

void ConflictDialog::buildComparison(const Task& local, const Task& server)
{
    QString lHtml = QString("<h3>%1 #%2</h3>").arg(local.tracker).arg(local.id);
    QString sHtml = QString("<h3>%1 #%2</h3>").arg(server.tracker).arg(server.id);

    auto addField = [&](QString lbl, QString lVal, QString sVal)
    {
        QString lDiff = getDiffHtml(lVal, sVal, "#1B5E20", "#D5F5E3");
        QString sDiff = getDiffHtml(sVal, lVal, "#0D47A1", "#BBDEFB");
        lHtml += QString("<p><b>%1:</b> %2</p>").arg(lbl, lDiff);
        sHtml += QString("<p><b>%1:</b> %2</p>").arg(lbl, sDiff);
    };

    if(local.subject != server.subject) addField("Subject", local.subject, server.subject);
    else lHtml += QString("<p><b>Subject:</b> %1</p>").arg(local.subject), sHtml += QString("<p><b>Subject:</b> %1</p>").arg(server.subject);

    if(local.description != server.description) addField("Description", local.description, server.description);
    else lHtml += QString("<p><b>Description:</b> %1</p>").arg(local.description), sHtml += QString("<p><b>Description:</b> %1</p>").arg(server.description);

    if(local.status != server.status) addField("Status", local.status, server.status);
    if(local.priority != server.priority) addField("Priority", local.priority, server.priority);
    if(local.dueDate != server.dueDate) addField("Due Date", local.dueDate, server.dueDate);

    ui->localData->setHtml(lHtml);
    ui->serverData->setHtml(sHtml);
}

void ConflictDialog::onLocalClicked()
{
    resultLocal = true;
    accept();
}

void ConflictDialog::onServerClicked()
{
    resultLocal = false;
    accept();
}
