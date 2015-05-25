#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logging.h"

/******************************************************************************/
MainWindow::MainWindow(const TestSettings &a_settings, QWidget *parent)
 : QMainWindow(parent)
 , ui(new Ui::MainWindow)
 , m_settings(a_settings)
{
    qCDebug(LogQtTestRunner);
    ui->setupUi(this);

    ui->treeView->setModel(&m_unittestmodel);
    //ui->treeView->expandAll();

    m_unitTestCollector.reset(new UnitTestCollector());

    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::finished,
                     this, &MainWindow::onFinished);
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::unitTestResult,
                     this, &MainWindow::onUnitTestResult);

    QObject::connect(ui->pbStart, &QPushButton::clicked,
                     this, &MainWindow::onStartClicked);
}

/******************************************************************************/
MainWindow::~MainWindow()
{
    qCDebug(LogQtTestRunner);
    delete ui;
}

/******************************************************************************/
void MainWindow::onStartClicked()
{
    m_unittestmodel.clear();

    m_unitTestCollector->start(m_settings);
}

/******************************************************************************/
void MainWindow::onUnitTestResult(int jobnr,
                                  const QString &testCase,
                                  const QString &testFunction,
                                  const QString &testResult)
{
    Q_UNUSED(jobnr);

    char buf[256];
    snprintf(buf, 256, "%-10s %-20s %-20s\n",
            testResult.toStdString().c_str(),
            testCase.toStdString().c_str(),
            testFunction.toStdString().c_str());

    //ui->listWidget->addItem(QString(buf).trimmed());
}

/******************************************************************************/
void MainWindow::onFinished()
{
    //ui->listWidget->addItem(QString("Finished"));
}

/******************************************************************************/
