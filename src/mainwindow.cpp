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
    ui->treeView->expandAll();



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
}

/******************************************************************************/
void MainWindow::onFinished()
{
}

/******************************************************************************/
