/*
Copyright (C) 2015 Henk van der Laak

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <QLoggingCategory>
#include <QMutex>

#include "unittestoutputhandler.h"
#include "unittestrunner.h"

Q_LOGGING_CATEGORY(LogQtTestRunnerCoreHandler, "QtTestRunnerCoreHandler")

QMutex g_access;

/******************************************************************************/
UnitTestOutputHandler::UnitTestOutputHandler(const QString &a_testSuiteName)
    : QObject()

    , m_unitTestRunner(nullptr)
    , m_testCase(nullptr)
    , m_testFunction(nullptr)
    , m_incident(nullptr)
    , m_message(nullptr)
    , m_benchmark(nullptr)
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
    m_testSuite.reset(new TestSuite());
    m_testSuite->m_name = a_testSuiteName;

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
    m_testSuite.reset();
}

/******************************************************************************/
void UnitTestOutputHandler::setUnitTestRunner(UnitTestRunner *a_unitTestRunner)
{
    m_unitTestRunner = a_unitTestRunner;
}

/******************************************************************************/
void UnitTestOutputHandler::processXmlLine(const QString &a_line)
{
    Locker lock(g_access);
    QRegularExpressionMatch match;

    if (!m_inTestCase)
    {
        // xml line
        match = re_xml.match(a_line);
        if (match.hasMatch())
        {
            qCDebug(LogQtTestRunnerCoreHandler, "---- XML ----");
            return;
        }

        // start testcase
        match = re_tc_start.match(a_line);
        if (match.hasMatch())
        {
            QString name = match.captured(1);

            qCDebug(LogQtTestRunnerCoreHandler, "---- TC: %s----",
                    match.captured(1).toLatin1().data());
            m_inTestCase = true;

            TestCase newtestcase;
            m_testSuite->m_testCases.append(newtestcase);
            m_testCase = &(m_testSuite->m_testCases.last());
            m_testCase->m_name = name;

            return;
        }
    }
    else
    {
        if (m_inEnvironment)
        {
            // end environment
            match = re_environment_end.match(a_line);
            if (match.hasMatch())
            {
                m_inEnvironment = false;
                qCDebug(LogQtTestRunnerCoreHandler, "---- ~ENV ----");
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
                    match = re_description_end.match(a_line);
                    if (match.hasMatch())
                    {
                        qCDebug(LogQtTestRunnerCoreHandler, "---- DESC ----");
                        m_inDescription = false;
                        return;
                    }
                }
                else
                {
                    // single line description
                    match = re_description.match(a_line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);

                        qCDebug(LogQtTestRunnerCoreHandler, "---- DESC %s ----",
                                match.captured(1).toLatin1().data());

                        m_message->m_description.append(text);

                        return;
                    }

                    // start description
                    match = re_description_start.match(a_line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);
                        qCDebug(LogQtTestRunnerCoreHandler, "---- DESC %s ----",
                                match.captured(1).toLatin1().data());
                        m_inDescription = true;

                        m_message->m_description.append(text+"\n");
                        return;
                    }

                    // end of message
                    match = re_message_end.match(a_line);
                    if (match.hasMatch())
                    {
                        qCDebug(LogQtTestRunnerCoreHandler, "---- ~MSG ----");
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
                    match = re_description_end.match(a_line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);

                        qCDebug(LogQtTestRunnerCoreHandler, "---- DESC %s ----",
                                match.captured(1).toLatin1().data());
                        m_inDescription = false;

                        m_incident->m_description.append(text);
                        return;
                    }
                    qCDebug(LogQtTestRunnerCoreHandler, "---- DESC %s ----",
                            a_line.toLatin1().data());

                    m_incident->m_description.append(a_line+"\n");

                    return;
                }
                else
                {
                    // single line description
                    match = re_description.match(a_line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);
                        m_incident->m_description.append(text);
                        qCDebug(LogQtTestRunnerCoreHandler, "---- DESC %s ----",
                                match.captured(1).toLatin1().data());
                        return;
                    }

                    // start description
                    match = re_description_start.match(a_line);
                    if (match.hasMatch())
                    {
                        QString text = match.captured(1);
                        qCDebug(LogQtTestRunnerCoreHandler, "---- DESC %s ----",
                                match.captured(1).toLatin1().data());
                        m_inDescription = true;

                        m_incident->m_description.append(text+"\n");
                        return;
                    }

                    // single line datatag
                    match = re_datatag.match(a_line);
                    if (match.hasMatch())
                    {
                        QString tag = match.captured(1);
                        qCDebug(LogQtTestRunnerCoreHandler, "---- TAG %s ----",
                                match.captured(1).toLatin1().data());

                        m_incident->m_datatag = tag;
                        return;
                    }

                    match = re_incident_end.match(a_line);
                    if (match.hasMatch())
                    {
                        qCDebug(LogQtTestRunnerCoreHandler, "---- ~INC ----");
                        m_inIncident = false;

                        m_incident->m_done = true;
                        m_incident = 0;

                        return;
                    }
                }
            }
            else
            {
                match = re_tf_end.match(a_line);
                if (match.hasMatch())
                {
                    //m_testFunctionName = "";
                    qCDebug(LogQtTestRunnerCoreHandler, "---- ~TF ----");
                    m_inTestFunction = false;

                    m_testFunction->m_done = true;

                    propagateResults();

                    if (m_unitTestRunner) emit m_unitTestRunner->endTestFunction(*m_testFunction);

                    m_testFunction = 0;
                    return;
                }

                // start message
                match = re_message_start.match(a_line);
                if (match.hasMatch())
                {
                    QString type = match.captured(1);
                    QString file = match.captured(2);
                    QString line = match.captured(3);

                    qCDebug(LogQtTestRunnerCoreHandler, "---- MSG: %s ----",
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
                match = re_incident.match(a_line);
                if (match.hasMatch())
                {
                    QString type = match.captured(1);
                    QString file = match.captured(2);
                    QString line = match.captured(3);

                    qCDebug(LogQtTestRunnerCoreHandler, "---- INC: %s ----",
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
                match = re_incident_start.match(a_line);
                if (match.hasMatch())
                {
                    QString type = match.captured(1);
                    QString file = match.captured(2);
                    QString line = match.captured(3);

                    qCDebug(LogQtTestRunnerCoreHandler, "---- INC: %s ----",
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
                match = re_benchmark.match(a_line);
                if (match.hasMatch())
                {
                    QString metric = match.captured(1);
                    QString tag = match.captured(2);
                    QString value = match.captured(3);
                    QString iterations = match.captured(4);

                    qCDebug(LogQtTestRunnerCoreHandler, "---- BENCH %s ----",
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
                match = re_duration.match(a_line);
                if (match.hasMatch())
                {
                    QString duration = match.captured(1);

                    qCDebug(LogQtTestRunnerCoreHandler, "---- DUR %s ----",
                            match.captured(1).toLatin1().data());

                    m_testFunction->m_duration = duration;
                    return;
                }
            }
        }
        else
        {
            // start environment
            match = re_environment_start.match(a_line);
            if (match.hasMatch())
            {
                m_inEnvironment = true;
                qCDebug(LogQtTestRunnerCoreHandler, "---- ENV ----");
                return;
            }

            // start test function
            match = re_tf_start.match(a_line);
            if (match.hasMatch())
            {
                QString name = match.captured(1);

                //m_testFunctionName = match.captured(1);
                qCDebug(LogQtTestRunnerCoreHandler, "---- TF: %s ----",
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
            match = re_duration.match(a_line);
            if (match.hasMatch())
            {
                QString duration = match.captured(1);
                qCDebug(LogQtTestRunnerCoreHandler, "---- Duration ----");

                m_testCase->m_duration = duration;
                return;
            }

            // end testcase
            match = re_tc_end.match(a_line);
            if (match.hasMatch())
            {
                //m_testCaseName = "";
                qCDebug(LogQtTestRunnerCoreHandler, "---- ~TC ----");
                m_inTestCase = false;

                m_testCase->m_done = true;
                m_testCase = 0;
                return;
            }
        }
    }

    //not processed, log
    //qCWarning(LogQtTestRunnerCoreHandler, "%s", line.toLatin1().data());
}

/******************************************************************************/
const TestSuite &UnitTestOutputHandler::getTestSuite() const
{
    return *(m_testSuite.data());
}

/******************************************************************************/
void UnitTestOutputHandler::propagateResults()
{
    qCDebug(LogQtTestRunnerCoreHandler, "---- Propragate ----");
    Q_ASSERT(m_testFunction != nullptr);
    Q_ASSERT(m_testCase != nullptr);
    Q_ASSERT(m_testSuite != nullptr);

    if (!hasTestFunctionPassed())
    {
        m_testFunction->m_pass = false;
        m_testCase->m_pass = false;
        m_testSuite->m_pass = false;
    }
}

/******************************************************************************/
bool UnitTestOutputHandler::hasTestFunctionPassed()
{
    bool pass = true;

    // Determine if test was ok
    for (auto it = m_testFunction->m_incidents.begin();
            it != m_testFunction->m_incidents.end();
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
    return pass;
}


/******************************************************************************/
