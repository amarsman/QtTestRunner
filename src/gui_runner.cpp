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
    QObject::connect(m_testManager, &TestManager::testSuiteChanged,
                     this, &GuiRunner::onTestSuiteChanged);
    QObject::connect(&m_unittestmodel, &UnitTestModel::refreshDone,
                     this, &GuiRunner::onRefreshDone);

    void refreshDone();

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
    ui->pbStart->setEnabled(true);
    m_testManager->start(m_settings);
}

/******************************************************************************/
void GuiRunner::onTestSuiteChanged(const TestSuite &a_testSuite)
{
    m_unittestmodel.refresh(a_testSuite);

}

/******************************************************************************/
void GuiRunner::onRefreshDone()
{
    ui->treeView->expandAll();
}

/******************************************************************************/
void GuiRunner::onFinished()
{
    ui->pbStart->setEnabled(true);
}

/******************************************************************************/
