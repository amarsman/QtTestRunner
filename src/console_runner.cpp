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
void ConsoleRunner::onUnitTestFound(const QString &a_path, int a_nrtests)
{
    qCDebug(LogQtTestRunner, "%s %d", a_path.toStdString().c_str(), a_nrtests);
}

/******************************************************************************/
void ConsoleRunner::onCollectionFinished()
{
    qCDebug(LogQtTestRunner, "Finished");
    emit finished();
}

/******************************************************************************/
void ConsoleRunner::onEndTestCase(const TestCase &testcase)
{
    Q_UNUSED(testcase);
}

/******************************************************************************/
void ConsoleRunner::onEndTestFunction(const TestFunction &testfunction)
{
    Locker lock(g_access);

    if (testfunction.m_done)
    {
        bool pass = true;

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

        fprintf(stderr, "%s%-35s%-35s%12.1lf  %s\n",
                pass ? STYLE_GREEN : STYLE_RED,
                testfunction.m_casename.toStdString().c_str(),
                testfunction.m_name.toStdString().c_str(),
                testfunction.m_duration.toDouble(),
                pass ? "OK" : "FAIL");

        // print messages
        for (auto it = testfunction.m_messages.begin();
             it != testfunction.m_messages.end();
             ++it)
        {
            const Message &message= *it;

            if (message.m_done && !message.m_description.isEmpty() && !pass)
            {
                fprintf(stderr, "    %s%s: %s\n",
                        STYLE_BLUE,
                        message.m_type.toStdString().c_str(),
                        message.m_description.toStdString().c_str());
            }
        }

        // print incidents
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
    fprintf(stderr, "%s", STYLE_DEFAULT);
}

/******************************************************************************/
