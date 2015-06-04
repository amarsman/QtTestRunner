#include <QFileInfo>
#include "unittestoutputhandler.h"
#include "logging.h"

/******************************************************************************/
UnitTestOutputHandler::UnitTestOutputHandler(UnitTestRunner *a_runner, QString a_testpath)
    : QObject()
    , m_runner(a_runner)
    , m_inTestCase(false)
    , m_inEnvironment(false)
    , m_inTestFunction(false)
    , m_inIncident(false)
    , m_inDuration(false)
    , m_inMessage(false)
    , m_inDescription(false)
    , m_inDataTag(false)
    , m_inBenchmarkResult(false)
    , m_testpath(a_testpath)
{
    qRegisterMetaType<TestCase>("TestCase");
    qRegisterMetaType<TestFunction>("TestFunction");

    re_xml              .setPattern("<\\?xml.*\\?>");
    re_tc_start         .setPattern("<TestCase name=\"(.*?)\">");
    re_tc_end           .setPattern("<\\/TestCase>");
    re_environment_start.setPattern("<Environment>");
    re_environment_end  .setPattern("<\\/Environment>");
    re_tf_start         .setPattern("<TestFunction name=\"(.*?)\">");
    re_tf_end           .setPattern("<\\/TestFunction>");
    re_incident         .setPattern("<Incident\\s+type=\"(.*?)\".*?\\/>");
    re_incident_start   .setPattern("<Incident\\s+type=\"(.*?)\".*?[^\\/]>");
    re_incident_end     .setPattern("<\\/Incident>");
    re_message_start   .setPattern("<Message\\s+type=\"(.*?)\".*?[^\\/]>");
    re_message_end     .setPattern("<\\/Message>");
    re_duration         .setPattern("<Duration.*?\\/>");
}

/******************************************************************************/
UnitTestOutputHandler::~UnitTestOutputHandler()
{

}

/******************************************************************************/
void UnitTestOutputHandler::processXmlLine(const QString &line)
{
    QString result = "";

    QRegularExpressionMatch match;

    if (!m_inTestCase)
    {
        // xml line
        match = re_xml.match(line);
        if (match.hasMatch())
        {
            qCDebug(LogQtTestRunnerCore, "---- XML ----");
            return;
        }

        // start testcase
        match = re_tc_start.match(line);
        if (match.hasMatch())
        {
            //m_testCaseName = match.captured(1);
            qCDebug(LogQtTestRunnerCore, "---- TC: %s----", match.captured(1).toStdString().c_str());
            m_inTestCase = true;
            return;
        }

    }
    else
    {
        if (m_inEnvironment)
        {
            // end environment
            match = re_environment_end.match(line);
            if (match.hasMatch())
            {
                m_inEnvironment = false;
                qCDebug(LogQtTestRunnerCore, "---- ~ENV ----");
                return;
            }

            return; // ignore anything else
        }
        else if (m_inTestFunction)
        {
            if (m_inMessage)
            {
                match = re_message_end.match(line);
                if (match.hasMatch())
                {
                    qCDebug(LogQtTestRunnerCore, "---- ~MSG ----");
                    m_inMessage= false;
                    return;
                }
            }
            else  if (m_inIncident)
            {
                match = re_incident_end.match(line);
                if (match.hasMatch())
                {
                    qCDebug(LogQtTestRunnerCore, "---- ~INC ----");
                    m_inIncident = false;
                    return;
                }
            }
            else
            {
                match = re_tf_end.match(line);
                if (match.hasMatch())
                {
                    //m_testFunctionName = "";
                    qCDebug(LogQtTestRunnerCore, "---- ~TF ----");
                    m_inTestFunction = false;
                    return;
                }

                // start message
                match = re_message_start.match(line);
                if (match.hasMatch())
                {
                    result = match.captured(1);
                    qCDebug(LogQtTestRunnerCore, "---- MSG: %s ----", match.captured(1).toStdString().c_str());
                    m_inMessage = true;
                    return;
                }

                // Single line incident
                match = re_incident.match(line);
                if (match.hasMatch())
                {
                    result = match.captured(1);
                    qCDebug(LogQtTestRunnerCore, "---- INC: %s ----", match.captured(1).toStdString().c_str());
                    return;
                }

                // start incident
                match = re_incident_start.match(line);
                if (match.hasMatch())
                {
                    result = match.captured(1);
                    qCDebug(LogQtTestRunnerCore, "---- INC: %s ----", match.captured(1).toStdString().c_str());
                    m_inIncident = true;
                    return;
                }

                // Single line duration
                match = re_duration.match(line);
                if (match.hasMatch())
                {
                    qCDebug(LogQtTestRunnerCore, "---- DUR ----");
                    return;
                }
            }
        }
        else
        {
            // start environment
            match = re_environment_start.match(line);
            if (match.hasMatch())
            {
                m_inEnvironment = true;
                qCDebug(LogQtTestRunnerCore, "---- ENV ----");
                return;
            }

            // start test function
            match = re_tf_start.match(line);
            if (match.hasMatch())
            {
                //m_testFunctionName = match.captured(1);
                qCDebug(LogQtTestRunnerCore, "---- TF: %s ----", match.captured(1).toStdString().c_str());
                m_inTestFunction = true;
                return;
            }

            // duration
            match = re_duration.match(line);
            if (match.hasMatch())
            {
                qCDebug(LogQtTestRunnerCore, "---- Duration ----");
                return;
            }

            // end testcase
            match = re_tc_end.match(line);
            if (match.hasMatch())
            {
                //m_testCaseName = "";
                qCDebug(LogQtTestRunnerCore, "---- ~TC ----");
                m_inTestCase = false;
                return;
            }
        }
    }

    //not processed
    qCDebug(LogQtTestRunnerCore, "%s", line.toStdString().c_str());
}

/******************************************************************************/
