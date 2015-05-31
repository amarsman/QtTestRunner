#include <QString>
#include <QRegularExpression>

#include "console_runner.h"
#include "logging.h"
#include "testmanager.h"

const char STYLE_DEFAULT[]    = "\033[0m";
const char STYLE_RED[]        = "\033[0;31m";
const char STYLE_GREEN[]      = "\033[0;32m";
const char STYLE_BLUE[]       = "\033[0;34m";
const char STYLE_GRAY[]       = "\033[0;37m";
const char STYLE_RED_BOLD[]   = "\033[1;31m";
const char STYLE_GREEN_BOLD[] = "\033[1;32m";
const char STYLE_BLUE_BOLD[]  = "\033[1;34m";

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
    QObject::connect(m_testManager, &TestManager::endTestCase,
                     this, &ConsoleRunner::onEndTestCase);
    QObject::connect(m_testManager, &TestManager::endTestFunction,
                     this, &ConsoleRunner::onEndTestFunction);

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
void ConsoleRunner::onEndTestCase(const TestCase &testcase)
{
}

/******************************************************************************/
void ConsoleRunner::onEndTestFunction(const TestFunction &testfunction)
{
    if (testfunction.m_done)
    {
        bool pass = true;

        for (auto it = testfunction.m_incidents.begin(); it != testfunction.m_incidents.end(); ++it)
        {
            const Incident &incident = *it;
            if (!incident.m_done ||
                    incident.m_type == "fail" ||
                    incident.m_type == "xpass")
            {
                pass = false;
            }
        }

        fprintf(stderr, "%s%-35s%-35s%-10s%s\n",
                pass ? STYLE_GREEN : STYLE_RED,
                testfunction.m_casename.toStdString().c_str(),
                testfunction.m_name.toStdString().c_str(),
                pass ? "OK" : "FAIL",
                STYLE_DEFAULT);

        for (auto it = testfunction.m_messages.begin(); it != testfunction.m_messages.end(); ++it)
        {
            if (it->m_done)
            {
                fprintf(stderr, "%s%s: %s%s\n",
                        STYLE_DEFAULT,
                        it->m_type.toStdString().c_str(),
                        //it->m_file.toStdString().c_str(),
                        //it->m_line.toStdString().c_str(),
                        it->m_description.toStdString().c_str(),
                        STYLE_DEFAULT);
            }
        }

        if (!pass)
        {
            for (auto it = testfunction.m_incidents.begin(); it != testfunction.m_incidents.end(); ++it)
            {
                const Incident &incident = *it;

                if (incident.m_done && (
                            incident.m_type == "fail" ||
                            incident.m_type == "xpass"))
                {
                    fprintf(stderr, "%s%s%s\n\n",
                            STYLE_BLUE,
                            incident.m_description.toStdString().c_str(),
                            STYLE_DEFAULT);
                }
            }
        }
    }
}

#if 0
/******************************************************************************/
void ConsoleRunner::onUnitTestResult(const TestCase &result)
{
    if (result.busy) return;

    fprintf(stderr, "%s %s::%s\n",
            result.type.toStdString().c_str(),
            result.name.toStdString().c_str(),
            result.testfunctionname.toStdString().c_str());


    for (auto it = result.incidents.begin(); it != result.incidents.end(); ++it)
    {
        const Incident &ref = *it;

        if (ref.type != "pass")
        {
            fprintf(stderr, "%s %s\n%s\n",
                    ref.file.toStdString().c_str(),
                    ref.line.toStdString().c_str(),
                    ref.message.toStdString().c_str());
        }
    }
}
#endif

/******************************************************************************/
