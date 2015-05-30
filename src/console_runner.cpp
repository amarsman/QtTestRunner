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
void ConsoleRunner::onUnitTestResult(const TestFunctionResult &result)
{
    if (result.busy) return;

    if (result.type == "pass")
    {
        fprintf(stderr, "PASS %s::%s\n",
                result.testcasename.toStdString().c_str(),
                result.testfunctionname.toStdString().c_str());
    }
    else
    {
        fprintf(stderr, "FAIL %s::%s  file=%s   line=%s\n",
                result.testcasename.toStdString().c_str(),
                result.testfunctionname.toStdString().c_str(),
                result.file.toStdString().c_str(),
                result.line.toStdString().c_str());
    }
    for (auto it = result.incidents.begin(); it != result.incidents.end(); ++it)
    {
        //fprintf(stderr, "INC  %s %s\n", it->msg_class.toStdString().c_str(), it->message.toStdString().c_str());
    }
    for (auto it = result.messages.begin(); it != result.messages.end(); ++it)
    {
        //fprintf(stderr, "MSG  %s %s\n", it->msg_class.toStdString().c_str(), it->message.toStdString().c_str());
    }
}

/******************************************************************************/
