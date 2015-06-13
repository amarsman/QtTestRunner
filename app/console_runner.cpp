#include <QLoggingCategory>

#include "console_runner.h"
#include "testmanager.h"

Q_DECLARE_LOGGING_CATEGORY(LogQtTestRunner)

static char STYLE_DEFAULT[]    = "\033[0m";
static char STYLE_BOLD[]       = "\033[1;30m";
static char STYLE_RED[]        = "\033[0;31m";
static char STYLE_RED_BOLD[]   = "\033[1;31m";
static char STYLE_GREEN[]      = "\033[0;32m";
static char STYLE_GREEN_BOLD[] = "\033[1;32m";
static char STYLE_BLUE[]       = "\033[0;34m";
static char STYLE_BLUE_BOLD[]  = "\033[1;34m";
static char STYLE_GRAY[]       = "\033[0;37m";

/******************************************************************************/
ConsoleRunner::ConsoleRunner(TestManager *a_testManager,
                             TestSettings *a_testSettings)
    : QObject()
    , m_testManager(a_testManager)
    , m_testSettings(a_testSettings)
    , m_totalNrTestsFound(0)
    , m_totalNrTestsRun(0)
    , m_totalNrTestsPassed(0)
    , m_allTestsOk(true)
{
    qCDebug(LogQtTestRunner);

    if (a_testSettings->no_colors)
    {
        STYLE_DEFAULT[0]    = 0;
        STYLE_BOLD[0]       = 0;
        STYLE_RED[0]        = 0;
        STYLE_RED_BOLD[0]   = 0;
        STYLE_GREEN[0]      = 0;
        STYLE_GREEN_BOLD[0] = 0;
        STYLE_BLUE[0]       = 0;
        STYLE_BLUE_BOLD[0]  = 0;
        STYLE_GRAY[0]       = 0;
    }
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
    QObject::connect(m_testManager, &TestManager::foundTestSuite,
                     this, &ConsoleRunner::onFoundTestSuite);
    QObject::connect(m_testManager, &TestManager::endTestFunction,
                     this, &ConsoleRunner::onEndTestFunction);
    QObject::connect(m_testManager, &TestManager::crashTestSuite,
                     this, &ConsoleRunner::onCrashTestSuite);
    QObject::connect(m_testManager, &TestManager::finishedTesting,
                     this, &ConsoleRunner::onFinishedTesting);

    m_testManager->start(m_testSettings);
}

/******************************************************************************/
void ConsoleRunner::onFoundTestSuite(const QString &a_testSuiteName, unsigned int a_nrTests)
{
    qCDebug(LogQtTestRunner, "%s", a_testSuiteName.toLatin1().data());
    m_totalNrTestsFound += a_nrTests;
}

/******************************************************************************/
void ConsoleRunner::onEndTestFunction(const TestFunction &a_testFunction)
{
    static int nrdots=0;
    Locker lock(g_access);

    int vv = m_testSettings->verbosity;

    if (a_testFunction.m_name == "initTestCase") return;
    if (a_testFunction.m_name == "cleanupTestCase") return;

    if (a_testFunction.m_done)
    {
        m_totalNrTestsRun++;

        bool pass = true;
        bool has_messages = false;

        // Determine if test was ok
        for (auto it = a_testFunction.m_incidents.begin();
                it != a_testFunction.m_incidents.end();
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

        if (!pass)
        {
            m_allTestsOk = false;
        }
        else
        {
            m_totalNrTestsPassed++;
        }

        // has messages?
        for (auto it = a_testFunction.m_messages.begin();
                it != a_testFunction.m_messages.end();
                ++it)
        {
            const Message &message= *it;

            if (message.m_done && !message.m_description.isEmpty() &&
                    (!message.m_description.contains("Authentication Rejected")))
            {
                has_messages = true;
            }
        }

        if (vv == 0 || ((vv == 1) &&  pass && !has_messages))
        {
            fprintf(stdout, "%s%s", pass ? STYLE_GREEN : STYLE_RED, pass ? "." : "E");
            nrdots++;
            if (nrdots > 80)
            {
                fprintf(stdout,"\n");
                nrdots=0;
            }
            fflush(stdout);
        }
        else
        {
            if (nrdots > 0)
            {
                fprintf(stdout, "\n");
                nrdots=0;
            }
            char buf[32];
            snprintf(buf,32,"%d/%d",
                     m_totalNrTestsRun, m_totalNrTestsFound * m_testSettings->repeat);
            fprintf(stdout, "%s%-9s %-40s%-75s %15s  %s\n",
                    pass ? STYLE_GREEN : STYLE_RED,
                    buf,
                    a_testFunction.m_casename.toLatin1().data(),
                    a_testFunction.m_name.toLatin1().data(),
                    a_testFunction.m_duration.toLatin1().data(),
                    pass ? "OK" : "FAIL");
        }

        // print messages
        if (vv >=1)
        {
            for (auto it = a_testFunction.m_messages.begin();
                    it != a_testFunction.m_messages.end();
                    ++it)
            {
                const Message &message= *it;

                if (message.m_done && !message.m_description.isEmpty())
                {
                    if (!message.m_description.contains("Authentication Rejected"))
                    {
                        if (nrdots > 0)
                        {
                            fprintf(stdout,"\n");
                            nrdots = 0;
                        }
                        fprintf(stdout, "    %s%s: %s\n",
                                STYLE_BLUE,
                                message.m_type.toLatin1().data(),
                                message.m_description.toLatin1().data());
                        nrdots = 0;
                    }
                }
            }
        }

        // print incidents
        if (vv >= 1)
        {
            for (auto it = a_testFunction.m_incidents.begin();
                    it != a_testFunction.m_incidents.end();
                    ++it)
            {
                const Incident &incident = *it;

                if (incident.m_done && !incident.m_description.isEmpty() && (
                            incident.m_type == "fail" ||
                            incident.m_type == "xpass"))
                {
                    if (!incident.m_datatag.isEmpty())
                    {
                            fprintf(stdout, "    %sDataset: %s\n",
                            pass ? STYLE_GREEN_BOLD : STYLE_RED_BOLD,
                            incident.m_datatag.toLatin1().data());
                    }
                    fprintf(stdout, "    %s%s %s\n",
                            pass ? STYLE_GREEN_BOLD : STYLE_RED_BOLD,
                            incident.m_file.toLatin1().data(),
                            incident.m_line.toLatin1().data());


                    QString desc = incident.m_description;
                    desc.replace(QString("\n"), QString("\n    "));
                    fprintf(stdout, "    %s%s\n",
                            pass ? STYLE_GREEN : STYLE_RED,
                            desc.toLatin1().data());
                }
            }
        }
    }
    fprintf(stdout, "%s", STYLE_DEFAULT);
}

/******************************************************************************/
void ConsoleRunner::onCrashTestSuite(const TestSuite &a_testSuite)
{
    fprintf(stdout,"\n%sCRASH in %s%s\n",
            STYLE_RED_BOLD, a_testSuite.m_name.toLatin1().data(),
            STYLE_DEFAULT);
    m_allTestsOk = false;
}

/******************************************************************************/
void ConsoleRunner::onFinishedTesting()
{
    qCDebug(LogQtTestRunner, "Finished");

    bool all_ok = (m_totalNrTestsFound * m_testSettings->repeat == m_totalNrTestsPassed);
    fprintf(stdout, "\n%d failed, ",    m_totalNrTestsRun - m_totalNrTestsPassed);
    fprintf(stdout, "%d passed, ",  m_totalNrTestsPassed);
    fprintf(stdout, "%d run, ",      m_totalNrTestsRun);
    fprintf(stdout, "%d found, ", m_totalNrTestsFound * m_testSettings->repeat);
    fprintf(stdout, "final result: %s%s%s\n",
            all_ok ? STYLE_GREEN_BOLD : STYLE_RED_BOLD ,
            all_ok ? "OK" : "FAIL",
            STYLE_DEFAULT);
    emit testingFinished();
}

/******************************************************************************/
