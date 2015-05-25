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
void MainTask::onRun()
{
    qCDebug(LogQtTestRunner);

    startCollecting();
}

/******************************************************************************/
void MainTask::startCollecting()
{
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::unitTestFound,
                     this, &MainTask::onUnitTestFound);
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::finished,
                     this, &MainTask::onCollectionFinished);
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::unitTestResult,
                     this, &MainTask::onUnitTestResult);

    m_unitTestCollector->start(m_settings);
}

/******************************************************************************/
void MainTask::onUnitTestFound(const QString &a_path)
{
    qCDebug(LogQtTestRunner, "%s", a_path.toStdString().c_str());
    fprintf(stderr, "%s\n", a_path.toStdString().c_str());
}

/******************************************************************************/
void MainTask::onCollectionFinished()
{
    qCDebug(LogQtTestRunner);
    fprintf(stderr, "Finished\n");
    emit finished();
}

/******************************************************************************/
void MainTask::onUnitTestResult(int jobnr,
                                const QString &testCase,
                                const QString &testFunction,
                                const QString &testResult)
{
    Q_UNUSED(jobnr);

    fprintf(stderr, "%-10s %-20s %-20s\n",
            testResult.toStdString().c_str(),
            testCase.toStdString().c_str(),
            testFunction.toStdString().c_str());
}

/******************************************************************************/
