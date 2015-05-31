#include "gui_runner.h"
#include "ui_mainwindow.h"
#include "logging.h"

/******************************************************************************/
GuiRunner::GuiRunner(TestManager *a_testManager,
                     TestSettings *a_settings)
 : QMainWindow()
 , ui(new Ui::MainWindow)
 , m_testManager(a_testManager)
 , m_settings(a_settings)
{
    qCDebug(LogQtTestRunner);
    ui->setupUi(this);

    QObject::connect(ui->pbStart, &QPushButton::clicked,
                     this, &GuiRunner::onStartClicked);

    ui->treeView->setModel(&m_unittestmodel);
    ui->treeView->expandAll();

    QObject::connect(m_testManager, &TestManager::finished,
                     this, &GuiRunner::onFinished);
    QObject::connect(m_testManager, &TestManager::testCaseChanged,
                     this, &GuiRunner::onTestCaseChanged);
}

/******************************************************************************/
GuiRunner::~GuiRunner()
{
    qCDebug(LogQtTestRunner);
    delete ui;
}

/******************************************************************************/
void GuiRunner::onStartClicked()
{
    m_testManager->start(m_settings);
}

/******************************************************************************/
void GuiRunner::onTestCaseChanged(const TestCase &a_testcase)
{
    m_unittestmodel.refresh(a_testcase);

#if 0
    QList<QStandardItem *> rowitems;
    rowitems << new QStandardItem(QString(testCase).trimmed());
    rowitems << new QStandardItem(QString(testFunction).trimmed());

    QStandardItem *item = m_unittestmodel.invisibleRootItem();
    item->appendRow(rowitems);

    QList<QStandardItem *> rowitems2;
    rowitems2 << new QStandardItem(QString(testResult).trimmed());


    rowitems.first()->appendRow(rowitems2);
    QModelIndex idx = m_unittestmodel.index(item->rowCount()-1, 0);
    ui->treeView->setExpanded(idx, (testResult == "fail"));
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);

    //ui->listWidget->addItem(QString(buf).trimmed());
#endif
}

/******************************************************************************/
void GuiRunner::onFinished()
{
}

/******************************************************************************/
