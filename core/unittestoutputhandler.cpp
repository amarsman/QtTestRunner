#include <QLoggingCategory>
#include <QMutex>

#include "unittestoutputhandler.h"
#include "unittestrunner.h"

Q_DECLARE_LOGGING_CATEGORY(LogQtTestRunnerCore)

QMutex g_access;

/******************************************************************************/
UnitTestOutputHandler::UnitTestOutputHandler(const QString &a_testSuiteName)
    : QObject()
    , m_runner(nullptr)
    , m_inTestCase(false)
    , m_inEnvironment(false)
    , m_inTestFunction(false)
    , m_inIncident(false)
    , m_inDuration(false)
    , m_inMessage(false)
    , m_inDescription(false)
    , m_inDataTag(false)
    , m_inBenchmarkResult(false)
{
    m_testSuite.m_name = a_testSuiteName;

    qRegisterMetaType<TestSuite>("TestSuite");
    qRegisterMetaType<TestCase>("TestCase");
    qRegisterMetaType<TestFunction>("TestFunction");

    re_xml              .setPattern("<\\?xml.*\\?>");
    re_tc_start         .setPattern("<TestCase name=\"(.*?)\">");
    re_tc_end           .setPattern("<\\/TestCase>");
    re_environment_start.setPattern("<Environment>");
    re_environment_end  .setPattern("<\\/Environment>");
    re_tf_start         .setPattern("<TestFunction name=\"(.*?)\">");
    re_tf_end           .setPattern("<\\/TestFunction>");
    re_incident         .setPattern("<Incident\\s+type=\"(.*?)\"\\s+file=\"(.*?)\"\\s+line=\"(.*?)\"\\s*\\/>");
    re_incident_start   .setPattern("<Incident\\s+type=\"(.*?)\"\\s+file=\"(.*?)\"\\s+line=\"(.*?)\"\\s*\\>");
    re_incident_end     .setPattern("<\\/Incident>");
    re_message_start    .setPattern("<Message\\s+type=\"(.*?)\"\\s+file=\"(.*?)\"\\s+line=\"(.*?)\"\\s*\\>");
    re_message_end      .setPattern("<\\/Message>");
    re_duration         .setPattern("<Duration\\s+msecs=\"(.*?)\".*?\\/>");
    re_description      .setPattern("<Description><!\\[CDATA\\[(.*)\\]\\]><\\/Description>");
    re_description_start.setPattern("<Description><!\\[CDATA\\[(.*)");
    re_description_end  .setPattern("(.*)]]><\\/Description>");
    re_datatag          .setPattern("<DataTag><!\\[CDATA\\[(.*)\\]\\]><\\/DataTag>");
    re_benchmark        .setPattern("<BenchmarkResult\\s+metric=\"(.*?)\"\\s+tag=\"(.*?)\"\\s+value=\"(.*?)\"\\s+iterations=\"(.*?)\".*?\\/>");
}

/******************************************************************************/
UnitTestOutputHandler::~UnitTestOutputHandler()
{

}

/******************************************************************************/
void UnitTestOutputHandler::setUnitTestRunner(UnitTestRunner *a_runner)
{
    m_runner = a_runner;
}

/******************************************************************************/
void UnitTestOutputHandler::processXmlLine(const QString &line)
{
    Locker lock(g_access);
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
            QString name = match.captured(1);

            qCDebug(LogQtTestRunnerCore, "---- TC: %s----",
                    match.captured(1).toLatin1().data());
            m_inTestCase = true;

            TestCase newtestcase;
            m_testSuite.m_testCases.append(newtestcase);
            m_testCase = &(m_testSuite.m_testCases.last());
            m_testCase->m_name = name;

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
                if (m_inDescription)
                {
                    // end of description
                    match = re_description_end.match(line);
                    if (match.hasMatch())
                    {
                        qCDebug(LogQtTestRunnerCore, "---- DESC ----");
                        m_inDescription = false;
                        return;
                    }
                }
                else
                {
                    // single line description
                    match = re_description.match(line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);

                        qCDebug(LogQtTestRunnerCore, "---- DESC %s ----",
                                match.captured(1).toLatin1().data());

                        m_message->m_description.append(text);

                        return;
                    }

                    // start description
                    match = re_description_start.match(line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);
                        qCDebug(LogQtTestRunnerCore, "---- DESC %s ----",
                                match.captured(1).toLatin1().data());
                        m_inDescription = true;

                        m_message->m_description.append(text+"\n");
                        return;
                    }

                    // end of message
                    match = re_message_end.match(line);
                    if (match.hasMatch())
                    {
                        qCDebug(LogQtTestRunnerCore, "---- ~MSG ----");
                        m_inMessage= false;
                        m_message->m_done = true;
                        return;
                    }
                }
            }
            else  if (m_inIncident)
            {
                if (m_inDescription)
                {
                    // end of description
                    match = re_description_end.match(line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);

                        qCDebug(LogQtTestRunnerCore, "---- DESC %s ----",
                                match.captured(1).toLatin1().data());
                        m_inDescription = false;

                        m_incident->m_description.append(text);
                        return;
                    }
                    qCDebug(LogQtTestRunnerCore, "---- DESC %s ----",
                            line.toLatin1().data());

                    m_incident->m_description.append(line+"\n");

                    return;
                }
                else
                {
                    // single line description
                    match = re_description.match(line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);
                        m_incident->m_description.append(text);
                        qCDebug(LogQtTestRunnerCore, "---- DESC %s ----",
                                match.captured(1).toLatin1().data());
                        return;
                    }

                    // start description
                    match = re_description_start.match(line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);
                        qCDebug(LogQtTestRunnerCore, "---- DESC %s ----",
                                match.captured(1).toLatin1().data());
                        m_inDescription = true;

                        m_incident->m_description.append(text+"\n");
                        return;
                    }

                    // single line datatag
                    match = re_datatag.match(line);
                    if (match.hasMatch())
                    {
                        QString tag = match.captured(1);
                        qCDebug(LogQtTestRunnerCore, "---- TAG %s ----",
                                match.captured(1).toLatin1().data());

                        m_incident->m_datatag = tag;
                        return;
                    }

                    match = re_incident_end.match(line);
                    if (match.hasMatch())
                    {
                        qCDebug(LogQtTestRunnerCore, "---- ~INC ----");
                        m_inIncident = false;

                        m_incident->m_done = true;
                        m_incident = 0;

                        return;
                    }
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

                    m_testFunction->m_done = true;
                    if (m_runner) emit m_runner->endTestFunction(*m_testFunction);

                    m_testFunction = 0;
                    return;
                }

                // start message
                match = re_message_start.match(line);
                if (match.hasMatch())
                {
                    QString type = match.captured(1);
                    QString file = match.captured(2);
                    QString line = match.captured(3);

                    qCDebug(LogQtTestRunnerCore, "---- MSG: %s ----",
                            match.captured(1).toLatin1().data());
                    m_inMessage = true;

                    Message message;
                    m_testFunction->m_messages.append(message);
                    m_message = &(m_testFunction->m_messages.last());
                    m_message->m_type = match.captured(1);
                    m_message->m_file = match.captured(2);
                    m_message->m_line = match.captured(3);

                    return;
                }

                // Single line incident
                match = re_incident.match(line);
                if (match.hasMatch())
                {
                    QString type = match.captured(1);
                    QString file = match.captured(2);
                    QString line = match.captured(3);

                    qCDebug(LogQtTestRunnerCore, "---- INC: %s ----",
                            match.captured(1).toLatin1().data());

                    Incident incident;
                    m_testFunction->m_incidents.append(incident);
                    m_incident = &(m_testFunction->m_incidents.last());
                    m_incident->m_type = match.captured(1);
                    m_incident->m_file = match.captured(2);
                    m_incident->m_line = match.captured(3);
                    m_incident->m_done = true;
                    return;
                }

                // start incident
                match = re_incident_start.match(line);
                if (match.hasMatch())
                {
                    QString type = match.captured(1);
                    QString file = match.captured(2);
                    QString line = match.captured(3);

                    qCDebug(LogQtTestRunnerCore, "---- INC: %s ----",
                            match.captured(1).toLatin1().data());
                    m_inIncident = true;

                    Incident incident;
                    m_testFunction->m_incidents.append(incident);
                    m_incident = &(m_testFunction->m_incidents.last());
                    m_incident->m_type = match.captured(1);
                    m_incident->m_file = match.captured(2);
                    m_incident->m_line = match.captured(3);

                    return;
                }

                // single line benchmark
                match = re_benchmark.match(line);
                if (match.hasMatch())
                {
                    QString metric = match.captured(1);
                    QString tag = match.captured(2);
                    QString value = match.captured(3);
                    QString iterations = match.captured(4);

                    qCDebug(LogQtTestRunnerCore, "---- BENCH %s ----",
                            match.captured(1).toLatin1().data());

                    Benchmark benchmark;
                    m_testFunction->m_benchmarks.append(benchmark);
                    m_benchmark = &(m_testFunction->m_benchmarks.last());
                    m_benchmark->m_metric = metric;
                    m_benchmark->m_tag = tag;
                    m_benchmark->m_value = value;
                    m_benchmark->m_iterations = iterations;
                    m_benchmark->m_done = true;

                    return;
                }

                // Single line duration
                match = re_duration.match(line);
                if (match.hasMatch())
                {
                    QString duration = match.captured(1);

                    qCDebug(LogQtTestRunnerCore, "---- DUR %s ----",
                            match.captured(1).toLatin1().data());

                    m_testFunction->m_duration = duration;
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
                QString name = match.captured(1);

                //m_testFunctionName = match.captured(1);
                qCDebug(LogQtTestRunnerCore, "---- TF: %s ----",
                        match.captured(1).toLatin1().data());
                m_inTestFunction = true;

                TestFunction testfunction;
                m_testCase->m_testfunctions.append(testfunction);
                m_testFunction = &(m_testCase->m_testfunctions.last());
                m_testFunction->m_name = name;
                m_testFunction->m_casename = m_testCase->m_name;
                return;
            }

            // duration
            match = re_duration.match(line);
            if (match.hasMatch())
            {
                QString duration = match.captured(1);
                qCDebug(LogQtTestRunnerCore, "---- Duration ----");

                m_testCase->m_duration = duration;
                return;
            }

            // end testcase
            match = re_tc_end.match(line);
            if (match.hasMatch())
            {
                //m_testCaseName = "";
                qCDebug(LogQtTestRunnerCore, "---- ~TC ----");
                m_inTestCase = false;

                m_testCase->m_done = true;
                m_testCase = 0;
                return;
            }
        }
    }

    //not processed, log
    //qCWarning(LogQtTestRunnerCore, "%s", line.toLatin1().data());
}

/******************************************************************************/
