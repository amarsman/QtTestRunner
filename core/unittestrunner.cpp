#include <QThreadPool>
#include <QProcess>
#include <QRegularExpression>

#include "unittestrunner.h"
#include "logging.h"

/******************************************************************************/
UnitTestRunner::UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore)
    : m_semaphore(a_semaphore), m_running(false), m_stopRequested(false), m_jobnr(0)
{
    qCDebug(LogQtTestRunnerCore);

    re_tc_start         .setPattern("<TestCase name=\"(.*?)\">");
    re_tc_end           .setPattern("<\\/TestCase>");
    re_environment_start.setPattern("<Environment>");
    re_environment_end  .setPattern("<\\/Environment>");
    re_tf_start         .setPattern("<TestFunction name=\"(.*?)\">");
    re_tf_end           .setPattern("<\\/TestFunction>");
    re_incident         .setPattern("<Incident\\s+type=\"(.*?)\".*?\\/>");
    re_incident_start   .setPattern("<Incident\\s+type=\"(.*?)\".*?[^\\/]>");
    re_incident_end     .setPattern("<\\/Incident>");
    re_duration         .setPattern("<Duration.*?\\/>");

    m_inTestcase      = false;
    m_testCaseName     = "";
    m_inEnvironment   = false;
    m_inTestFunction  = false;
    m_testFunctionName = "";
    m_inIncident      = false;
}

/******************************************************************************/
UnitTestRunner::~UnitTestRunner()
{
    qCDebug(LogQtTestRunnerCore);
}

/******************************************************************************/
void UnitTestRunner::start(int a_jobnr, const QString &a_unitTest)
{
    qCDebug(LogQtTestRunnerCore);
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
    qCDebug(LogQtTestRunnerCore);
    if (!m_running) return;

    qDebug("FileFinder Stopping...");
    m_stopRequested = true;
}

/******************************************************************************/
void UnitTestRunner::processXmlLine(const QString &line)
{
    QString result = "";


    QRegularExpressionMatch match;

    if (!m_inTestcase)
    {
        match = re_tc_start.match(line);
        if (match.hasMatch())
        {
            m_testCaseName = match.captured(1);
            m_inTestcase = true;
            return;
        }

        match = re_duration.match(line);
        if (match.hasMatch())
        {
            qCDebug(LogQtTestRunnerCore, "Duration");
            return;
        }
    }
    else // in testcase
    {
        match = re_tc_end.match(line);
        if (match.hasMatch())
        {
            m_testCaseName = "";
            m_inTestcase = false;
            return;
        }

        if (!m_inEnvironment)
        {
            match = re_environment_start.match(line);
            if (match.hasMatch())
            {
                m_inEnvironment = true;
                return;
            }
        }
        else // in_environment
        {
            match = re_environment_end.match(line);
            if (match.hasMatch())
            {
                m_inEnvironment = false;
                return;
            }
            return; // ignore anything else
        }

        if (!m_inTestFunction)
        {
            match = re_tf_start.match(line);
            if (match.hasMatch())
            {
                m_testFunctionName = match.captured(1);

                m_inTestFunction = true;
                return;
            }

            match = re_duration.match(line);
            if (match.hasMatch())
            {
                qCDebug(LogQtTestRunnerCore, "Duration");
                return;
            }
        }
        else // in testfunction
        {
            if (!m_inIncident)
            {
                match = re_tf_end.match(line);
                if (match.hasMatch())
                {
                    m_testFunctionName = "";
                    m_inTestFunction = false;
                    return;
                }

                // Single line incident
                match = re_incident.match(line);
                if (match.hasMatch())
                {
                    result = match.captured(1);
                    qCDebug(LogQtTestRunnerCore, "Incident");

                    emit unitTestResult(m_jobnr, m_testCaseName, m_testFunctionName, result);
                    return;
                }

                match = re_incident_start.match(line);
                if (match.hasMatch())
                {
                    result = match.captured(1);
                    m_inIncident = true;

                    emit unitTestResult(m_jobnr, m_testCaseName, m_testFunctionName, result);

                    return;
                }

                // Single line duration
                match = re_duration.match(line);
                if (match.hasMatch())
                {
                    qCDebug(LogQtTestRunnerCore, "Duration");
                    return;
                }
            }
            else // in incident
            {
                match = re_incident_end.match(line);
                if (match.hasMatch())
                {
                    m_inIncident = false;

                    return;
                }
            }
        }
    }

    //not processed
    //qDebug(line.toStdString().c_str());
}

/******************************************************************************/
void UnitTestRunner::run()
{
    qCDebug(LogQtTestRunnerCore, "Starting");

    if (m_running)
    {
        return;
    }

    m_running = true;

    qCDebug(LogQtTestRunnerCore, "%s ", m_unitTest.toStdString().c_str());

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
    qCDebug(LogQtTestRunnerCore, "Finished %s", result_ok ? "OK":"NOK");
    m_semaphore->release();
}

/******************************************************************************/
