#include <QString>
#include <QRegularExpression>

#include "console_runner.h"
#include "logging.h"
#include "testmanager.h"

const char STYLE_DEFAULT[]    = "\033[0m";
const char STYLE_BOLD[]       = "\033[1;30m";
const char STYLE_RED[]        = "\033[0;31m";
const char STYLE_RED_BOLD[]   = "\033[1;31m";
const char STYLE_GREEN[]      = "\033[0;32m";
const char STYLE_GREEN_BOLD[] = "\033[1;32m";
const char STYLE_BLUE[]       = "\033[0;34m";
const char STYLE_BLUE_BOLD[]  = "\033[1;34m";
const char STYLE_GRAY[]       = "\033[0;37m";

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
}

/******************************************************************************/
void ConsoleRunner::onCollectionFinished()
{
    fprintf(stderr,"\n");
    qCDebug(LogQtTestRunner, "Finished");
    emit finished();
}

/******************************************************************************/
void ConsoleRunner::onEndTestCase(const TestCase &testcase)
{
    Q_UNUSED(testcase);
}

/******************************************************************************/
// Verbosity:
// 0 - dots only.
// 1 - (default) failures only, including messages on passed tests
// 2 - full line per test
// 3 - include setup/teardown
/******************************************************************************/
void ConsoleRunner::onEndTestFunction(const TestFunction &testfunction)
{
    static int nrdots=0;
    Locker lock(g_access);

    int vv = m_settings->verbosity;

    if (vv <= 2 && testfunction.m_name == "initTestCase") return;
    if (vv <= 2 && testfunction.m_name == "cleanupTestCase") return;

    if (testfunction.m_done)
    {
        bool pass = true;
        bool has_messages = false;

        // Determine if test was ok
        for (auto it = testfunction.m_incidents.begin();
             it != testfunction.m_incidents.end();
             ++it)
        {
            const Incident &incident = *it;
            if (!incident.m_done ||
                    incident.m_type == "fail" ||
                    incident.m_type == "xpass")
            {
                pass = false;
            }
        }

        // has messages?
        for (auto it = testfunction.m_messages.begin();
             it != testfunction.m_messages.end();
             ++it)
        {
            const Message &message= *it;

            if (message.m_done && !message.m_description.isEmpty() && (!message.m_description.contains("Authentication Rejected")))
            {
                has_messages = true;
            }
        }

        if (vv == 0 || ((vv == 1) &&  pass && !has_messages))
        {
            fprintf(stderr, "%s%s", pass ? STYLE_GREEN : STYLE_RED, pass ? "." : "E");
            nrdots++;
            if (nrdots > 80)
            {
                fprintf(stderr,"\n");
                nrdots=0;
            }
            fflush(stderr);
        }
        else
        {
            if (nrdots > 0)
            {
                fprintf(stderr, "\n");
                nrdots=0;
            }
            fprintf(stderr, "%s%-40s%-75s %15s  %s\n",
                    pass ? STYLE_GREEN : STYLE_RED,
                    testfunction.m_casename.toStdString().c_str(),
                    testfunction.m_name.toStdString().c_str(),
                    testfunction.m_duration.toStdString().c_str(),
                    pass ? "OK" : "FAIL");
        }

        // print messages
        if (vv >=1)
        {
            for (auto it = testfunction.m_messages.begin();
                 it != testfunction.m_messages.end();
                 ++it)
            {
                const Message &message= *it;

                if (message.m_done && !message.m_description.isEmpty())
                {
                    if (!message.m_description.contains("Authentication Rejected"))
                    {
                        if (nrdots > 0)
                        {
                            fprintf(stderr,"\n");
                            nrdots = 0;
                        }
                        fprintf(stderr, "    %s%s: %s\n",
                                STYLE_BLUE,
                                message.m_type.toStdString().c_str(),
                                message.m_description.toStdString().c_str());
                        nrdots = 0;
                    }
                }
            }
        }

        // print incidents
        if (vv >= 1)
        {
            for (auto it = testfunction.m_incidents.begin();
                 it != testfunction.m_incidents.end();
                 ++it)
            {
                const Incident &incident = *it;

                if (incident.m_done && !incident.m_description.isEmpty() && (
                            incident.m_type == "fail" ||
                            incident.m_type == "xpass"))
                {
                    fprintf(stderr, "    %s%s %s\n",
                            pass ? STYLE_GREEN_BOLD : STYLE_RED_BOLD,
                            incident.m_file.toStdString().c_str(),
                            incident.m_line.toStdString().c_str());

                    QString desc = incident.m_description;
                    desc.replace(QString("\n"), QString("\n    "));
                    fprintf(stderr, "    %s%s\n",
                            pass ? STYLE_GREEN : STYLE_RED,
                            desc.toStdString().c_str());
                }
            }
        }
    }
    fprintf(stderr, "%s", STYLE_DEFAULT);
}

/******************************************************************************/
