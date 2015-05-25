#include <QString>
#include <QRegularExpression>

#include "maintask.h"
#include "logging.h"
#include "unittestcollector.h"


/******************************************************************************/
MainTask::MainTask(QObject *parent, const TestSettings &a_settings)
    : QObject(parent)
    , m_settings(a_settings)
{
    qCDebug(LogQtTestRunner);
    m_unitTestCollector.reset(new UnitTestCollector());
}


/******************************************************************************/
MainTask::~MainTask()
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
void MainTask::run()
{
    qCDebug(LogQtTestRunner);

    startCollecting();
}

/******************************************************************************/
void MainTask::startCollecting()
{
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::unittestFound,
                     this, &MainTask::onUnitTestFound);
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::collectionFinished,
                     this, &MainTask::onCollectionFinished);
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::unitTestResult,
                     this, &MainTask::onUnitTestResult);

    m_unitTestCollector->start(m_settings);
}

/******************************************************************************/
void MainTask::onUnitTestFound(const QString &a_path)
{
    qCDebug(LogQtTestRunner, "%s", a_path.toStdString().c_str());
}

/******************************************************************************/
void MainTask::onCollectionFinished()
{
    qCDebug(LogQtTestRunner);
    emit finished();
}

/******************************************************************************/
void MainTask::onUnitTestResult(int jobnr, const QString &testResult, bool ok)
{
    fprintf(stderr, "%d %s %s\n", jobnr, testResult.toStdString().c_str(), ok ? "OK" : "NOK");
}

/******************************************************************************/
