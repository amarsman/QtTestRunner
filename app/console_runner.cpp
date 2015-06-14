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
    , m_nrDots(0)
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
void ConsoleRunner::onFoundTestSuite(const QString &a_testSuiteName, unsigned int m_nrTestFunctions)
{
    qCDebug(LogQtTestRunner, "%s %d", a_testSuiteName.toLatin1().data(), m_nrTestFunctions);
}

/******************************************************************************/
void ConsoleRunner::onEndTestFunction(const TestFunction &a_testFunction, unsigned int m_testFunctionNr)
{
    qCDebug(LogQtTestRunner, "%s", a_testFunction.m_name.toLatin1().data());
    printTestFunctionResult(a_testFunction, m_testFunctionNr);
}

/******************************************************************************/
void ConsoleRunner::onCrashTestSuite(const TestSuite &a_testSuite)
{
    fprintf(stdout,"\n%sCRASH in %s%s\n",
            STYLE_RED_BOLD, a_testSuite.m_name.toLatin1().data(),
            STYLE_DEFAULT);
}

/******************************************************************************/
void ConsoleRunner::onFinishedTesting()
{
    qCDebug(LogQtTestRunner, "Finished");

    unsigned int nrFoundTestFunctions  = m_testManager->getNrFoundTestFunctions();
    unsigned int nrRunTestFunctions    = m_testManager->getNrRunTestFunctions();
    unsigned int nrPassedTestFunctions = m_testManager->getNrPassedFunctions();
    unsigned int nrFailedTestFunctinos = m_testManager->getNrFailedTestFunctions();

    bool all_ok = (nrPassedTestFunctions > 0) && (nrFoundTestFunctions == nrPassedTestFunctions);
    fprintf(stdout, "\n%d found, ", nrFoundTestFunctions);
    fprintf(stdout, "%d run, ",     nrRunTestFunctions);
    fprintf(stdout, "%d passed, ",  nrPassedTestFunctions);
    fprintf(stdout, "%d failed\n",    nrFailedTestFunctinos);
    fprintf(stdout, "%sFinal result: %s%s\n\n",
            all_ok ? STYLE_GREEN_BOLD : STYLE_RED_BOLD ,
            all_ok ? "OK" : "FAIL",
            STYLE_DEFAULT);

    emit testingFinished(all_ok ? 0 : 1);
}

/******************************************************************************/
void ConsoleRunner::printTestFunctionResult(const TestFunction &a_testFunction,
                                            unsigned int a_testFunctionNr)
{
    Locker lock(g_access);

    int vv = m_testSettings->verbosity;

    bool has_messages = false;
    unsigned int nrTestFunctions = m_testManager->getNrFoundTestFunctions();

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

    if (vv == 0 || ((vv == 1) &&  a_testFunction.m_pass && !has_messages))
    {
        fprintf(stdout, "%s%s", a_testFunction.m_pass ? STYLE_GREEN : STYLE_RED, a_testFunction.m_pass ? "." : "E");
        m_nrDots++;
        if (m_nrDots > 80)
        {
            fprintf(stdout,"\n");
            m_nrDots=0;
        }
        fflush(stdout);
    }
    else
    {
        if (m_nrDots > 0)
        {
            fprintf(stdout, "\n");
            m_nrDots=0;
        }
        char buf[32];
        snprintf(buf,32,"%d/%d",
                 a_testFunctionNr, nrTestFunctions);//m_totalNrTestsRun, m_totalNrTestsFound * m_testSettings->repeat);
        fprintf(stdout, "%s%-9s %-40s%-75s %15s  %s\n",
                a_testFunction.m_pass ? STYLE_GREEN : STYLE_RED,
                buf,
                a_testFunction.m_casename.toLatin1().data(),
                a_testFunction.m_name.toLatin1().data(),
                a_testFunction.m_duration.toLatin1().data(),
                a_testFunction.m_pass ? "OK" : "FAIL");
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
                    if (m_nrDots > 0)
                    {
                        fprintf(stdout,"\n");
                        m_nrDots = 0;
                    }
                    fprintf(stdout, "    %s%s: %s\n",
                            STYLE_BLUE,
                            message.m_type.toLatin1().data(),
                            message.m_description.toLatin1().data());
                    m_nrDots = 0;
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
                            a_testFunction.m_pass ? STYLE_GREEN_BOLD : STYLE_RED_BOLD,
                            incident.m_datatag.toLatin1().data());
                }
                fprintf(stdout, "    %s%s %s\n",
                        a_testFunction.m_pass ? STYLE_GREEN_BOLD : STYLE_RED_BOLD,
                        incident.m_file.toLatin1().data(),
                        incident.m_line.toLatin1().data());


                QString desc = incident.m_description;
                desc.replace(QString("\n"), QString("\n    "));
                fprintf(stdout, "    %s%s\n",
                        a_testFunction.m_pass ? STYLE_GREEN : STYLE_RED,
                        desc.toLatin1().data());
            }
        }
    }


    // Make sure to revert to default style
    fprintf(stdout, "%s", STYLE_DEFAULT);
}

/******************************************************************************/
