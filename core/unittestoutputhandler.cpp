#include <QFileInfo>
#include <QMutex>
#include "unittestoutputhandler.h"
#include "logging.h"

QMutex g_access;

/******************************************************************************/
UnitTestOutputHandler::UnitTestOutputHandler()
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
            if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
            return;
        }

        // start testcase
        match = re_tc_start.match(line);
        if (match.hasMatch())
        {
            QString name = match.captured(1);

            qCDebug(LogQtTestRunnerCore, "---- TC: %s----",
                    match.captured(1).toStdString().c_str());
            m_inTestCase = true;

            TestCase newtestcase;
            m_testSuite.m_testCases.append(newtestcase);
            m_testCase = &(m_testSuite.m_testCases.last());
            m_testCase->m_name = name;

            if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                return;
            }

            if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                        if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                                match.captured(1).toStdString().c_str());

                        m_message->m_description.append(text);

                        if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                        return;
                    }

                    // start description
                    match = re_description_start.match(line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);
                        qCDebug(LogQtTestRunnerCore, "---- DESC %s ----",
                                match.captured(1).toStdString().c_str());
                        m_inDescription = true;

                        m_message->m_description.append(text+"\n");
                        if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                        return;
                    }

                    // end of message
                    match = re_message_end.match(line);
                    if (match.hasMatch())
                    {
                        qCDebug(LogQtTestRunnerCore, "---- ~MSG ----");
                        m_inMessage= false;
                        m_message->m_done = true;
                        if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                                match.captured(1).toStdString().c_str());
                        m_inDescription = false;

                        m_incident->m_description.append(text);
                        if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                        return;
                    }
                    qCDebug(LogQtTestRunnerCore, "---- DESC %s ----",
                            line.toStdString().c_str());

                    m_incident->m_description.append(line+"\n");

                    if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                                match.captured(1).toStdString().c_str());
                        if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                        return;
                    }

                    // start description
                    match = re_description_start.match(line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);
                        qCDebug(LogQtTestRunnerCore, "---- DESC %s ----",
                                match.captured(1).toStdString().c_str());
                        m_inDescription = true;

                        m_incident->m_description.append(text+"\n");
                        if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                        return;
                    }

                    // single line datatag
                    match = re_datatag.match(line);
                    if (match.hasMatch())
                    {
                        QString tag = match.captured(1);
                        qCDebug(LogQtTestRunnerCore, "---- TAG %s ----",
                                match.captured(1).toStdString().c_str());

                        m_incident->m_datatag = tag;
                        if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                        return;
                    }

                    match = re_incident_end.match(line);
                    if (match.hasMatch())
                    {
                        qCDebug(LogQtTestRunnerCore, "---- ~INC ----");
                        m_inIncident = false;

                        m_incident->m_done = true;
                        m_incident = 0;

                        if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                    if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);

                    m_testFunction = 0;
                    if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                            match.captured(1).toStdString().c_str());
                    m_inMessage = true;

                    Message message;
                    m_testFunction->m_messages.append(message);
                    m_message = &(m_testFunction->m_messages.last());
                    m_message->m_type = match.captured(1);
                    m_message->m_file = match.captured(2);
                    m_message->m_line = match.captured(3);

                    if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                            match.captured(1).toStdString().c_str());

                    Incident incident;
                    m_testFunction->m_incidents.append(incident);
                    m_incident = &(m_testFunction->m_incidents.last());
                    m_incident->m_type = match.captured(1);
                    m_incident->m_file = match.captured(2);
                    m_incident->m_line = match.captured(3);
                    m_incident->m_done = true;
                    if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                            match.captured(1).toStdString().c_str());
                    m_inIncident = true;

                    Incident incident;
                    m_testFunction->m_incidents.append(incident);
                    m_incident = &(m_testFunction->m_incidents.last());
                    m_incident->m_type = match.captured(1);
                    m_incident->m_file = match.captured(2);
                    m_incident->m_line = match.captured(3);

                    if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                            match.captured(1).toStdString().c_str());

                    Benchmark benchmark;
                    m_testFunction->m_benchmarks.append(benchmark);
                    m_benchmark = &(m_testFunction->m_benchmarks.last());
                    m_benchmark->m_metric = metric;
                    m_benchmark->m_tag = tag;
                    m_benchmark->m_value = value;
                    m_benchmark->m_iterations = iterations;
                    m_benchmark->m_done = true;

                    if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                    return;
                }

                // Single line duration
                match = re_duration.match(line);
                if (match.hasMatch())
                {
                    QString duration = match.captured(1);

                    qCDebug(LogQtTestRunnerCore, "---- DUR %s ----",
                            match.captured(1).toStdString().c_str());

                    m_testFunction->m_duration = duration;
                    if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                return;
            }

            // start test function
            match = re_tf_start.match(line);
            if (match.hasMatch())
            {
                QString name = match.captured(1);

                //m_testFunctionName = match.captured(1);
                qCDebug(LogQtTestRunnerCore, "---- TF: %s ----",
                        match.captured(1).toStdString().c_str());
                m_inTestFunction = true;

                TestFunction testfunction;
                m_testCase->m_testfunctions.append(testfunction);
                m_testFunction = &(m_testCase->m_testfunctions.last());
                m_testFunction->m_name = name;
                m_testFunction->m_casename = m_testCase->m_name;
                if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                return;
            }

            // duration
            match = re_duration.match(line);
            if (match.hasMatch())
            {
                QString duration = match.captured(1);
                qCDebug(LogQtTestRunnerCore, "---- Duration ----");

                m_testCase->m_duration = duration;
                if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
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
                if (m_runner) emit m_runner->endTestCase(*m_testCase);
                if (m_runner) emit m_runner->testSuiteChanged(m_testSuite);
                return;
            }
        }
    }

    //not processed, log
    qCWarning(LogQtTestRunnerCore, "%s", line.toStdString().c_str());
}

/******************************************************************************/
