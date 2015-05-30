#include <QString>
#include <QRegularExpression>

#include "console_runner.h"
#include "logging.h"
#include "testmanager.h"


/******************************************************************************/
ConsoleRunner::ConsoleRunner(TestManager *a_testManager,
                             TestSettings *a_settings)
    : QObject()
    , m_testManager(a_testManager)
    , m_settings(a_settings)
{
    qCDebug(LogQtTestRunner);
}


/******************************************************************************/
ConsoleRunner::~ConsoleRunner()
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
void ConsoleRunner::onRun()
{
    qCDebug(LogQtTestRunner);

    startCollecting();
}

/******************************************************************************/
void ConsoleRunner::startCollecting()
{
    QObject::connect(m_testManager, &TestManager::unitTestFound,
                     this, &ConsoleRunner::onUnitTestFound);
    QObject::connect(m_testManager, &TestManager::finished,
                     this, &ConsoleRunner::onCollectionFinished);
    QObject::connect(m_testManager, &TestManager::unitTestResult,
                     this, &ConsoleRunner::onUnitTestResult);

    m_testManager->start(m_settings);
}

/******************************************************************************/
void ConsoleRunner::onUnitTestFound(const QString &a_path)
{
    qCDebug(LogQtTestRunner, "%s", a_path.toStdString().c_str());
    fprintf(stderr, "%s\n", a_path.toStdString().c_str());
}

/******************************************************************************/
void ConsoleRunner::onCollectionFinished()
{
    qCDebug(LogQtTestRunner);
    fprintf(stderr, "Finished\n");
    emit finished();
}

/******************************************************************************/
void ConsoleRunner::onUnitTestResult(int jobnr,
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
