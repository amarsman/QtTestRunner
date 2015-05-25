#include <QThreadPool>
#include <QProcess>
#include <QRegularExpression>

#include "unittestrunner.h"
#include "logging.h"

/******************************************************************************/
UnitTestRunner::UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore)
    : m_semaphore(a_semaphore), m_running(false), m_stopRequested(false), m_jobnr(0)
{
    qCDebug(LogQtTestRunner);

    re_tc_start.setPattern("<TestCase name=\"(.*?)\">");
    re_tc_end.setPattern("<\\/TestCase>");
    re_environment_start.setPattern("<Environment>");
    re_environment_end.setPattern("<\\/Environment>");
    re_tf_start.setPattern("<TestFunction name=\"(.*?)\">");
    re_tf_end.setPattern("<\\/TestFunction>");
    re_incident.setPattern("<Incident\\s+type=\"(.*?)\".*?\\/>");
    re_incident_start.setPattern("<Incident\\s+type=\"(.*?)\".*?[^\\/]>");
    re_incident_end.setPattern("<\\/Incident>");
    re_duration.setPattern("<Duration.*?\\/>");

    in_testcase = false;
    testCaseName = "";
    in_environment = false;
    in_testfunction = false;
    testFunctionName = "";
    in_incident = false;
}

/******************************************************************************/
UnitTestRunner::~UnitTestRunner()
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
void UnitTestRunner::start(int a_jobnr, const QString &a_unitTest)
{
    qCDebug(LogQtTestRunner);
    if (m_running)
    {
        return;
    }

    m_unitTest = a_unitTest;
    m_stopRequested = false;
    m_jobnr = a_jobnr;

    QThreadPool::globalInstance()->start(this);}

/******************************************************************************/
void UnitTestRunner::stop()
{
    qCDebug(LogQtTestRunner);
    if (!m_running) return;

    qDebug("FileFinder Stopping...");
    m_stopRequested = true;
}

/******************************************************************************/
void UnitTestRunner::processXmlLine(const QString &line)
{
    QString result = "";


    QRegularExpressionMatch match;

    if (!in_testcase)
    {
        match = re_tc_start.match(line);
        if (match.hasMatch())
        {
            testCaseName = match.captured(1);
            in_testcase = true;
            return;
        }
    }
    else // in testcase
    {
        match = re_tc_end.match(line);
        if (match.hasMatch())
        {
            testCaseName = "";
            in_testcase = false;
            return;
        }
    }

    if (!in_environment)
    {
        match = re_environment_start.match(line);
        if (match.hasMatch())
        {
            in_environment = true;
            return;
        }
    }
    else // in_environment
    {
        match = re_environment_end.match(line);
        if (match.hasMatch())
        {
            in_environment = false;
        }
        return;
    }

    if (!in_testfunction)
    {
        match = re_tf_start.match(line);
        if (match.hasMatch())
        {
            testFunctionName = match.captured(1);

            in_testfunction = true;
            return;
        }
    }
    else // in testfunction
    {
        if (!in_incident)
        {
            match = re_tf_end.match(line);
            if (match.hasMatch())
            {
                testFunctionName = "";
                in_testfunction = false;
                return;
            }

            // Single line incident
            match = re_incident.match(line);
            if (match.hasMatch())
            {
                result = match.captured(1);
                qCDebug(LogQtTestRunner, "Incident");

                emit unitTestResult(m_jobnr, testCaseName, testFunctionName, result);
            }

            match = re_incident_start.match(line);
            if (match.hasMatch())
            {
                result = match.captured(1);
                in_incident = true;

                emit unitTestResult(m_jobnr, testCaseName, testFunctionName, result);

                return;
            }

            // Single line duration
            match = re_duration.match(line);
            if (match.hasMatch())
            {
                qCDebug(LogQtTestRunner, "Duration");
            }
        }
        else // in incident
        {
                match = re_incident_end.match(line);
                if (match.hasMatch())
                {
                    in_incident = false;

                    return;
                }
        }
    }
}

/******************************************************************************/
void UnitTestRunner::run()
{
    qCDebug(LogQtTestRunner, "Starting");

    if (m_running)
    {
        return;
    }

    m_running = true;

    qCDebug(LogQtTestRunner, "%s ", m_unitTest.toStdString().c_str());

    QScopedPointer<QProcess> process(new QProcess());

    process->start(m_unitTest, QStringList() << "-xml");//, QStringList() << "-datatags");
    process->waitForStarted();
    while (process->waitForReadyRead())
    {
        while (process->canReadLine())
        {
            QString line = QString(process->readLine());
            processXmlLine(line);
        }
    }

    process->waitForFinished();

    bool result_ok = true;
    QProcess::ExitStatus status = process->exitStatus();
    if (status  != QProcess::NormalExit)
    {
        result_ok = false;
    }
    else if (0 != process->exitCode())
    {
        result_ok = false;
    }

    m_running = false;
    qCDebug(LogQtTestRunner, "Finished %s", result_ok ? "OK":"NOK");
    m_semaphore->release();
}

/******************************************************************************/
