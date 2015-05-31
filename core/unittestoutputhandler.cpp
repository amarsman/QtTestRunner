#include <QFileInfo>
#include "unittestoutputhandler.h"
#include "logging.h"

/******************************************************************************/
UnitTestOutputHandler::UnitTestOutputHandler(UnitTestRunner *a_runner, QString a_testpath)
    : QObject(), QXmlDefaultHandler()
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
}

/******************************************************************************/
UnitTestOutputHandler::~UnitTestOutputHandler()
{

}

/******************************************************************************/
bool UnitTestOutputHandler::startElement(const QString & namespaceURI,
                                         const QString & localName,
                                         const QString & qName,
                                         const QXmlAttributes & atts)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if (!m_inTestCase && qName == "TestCase")
    {
        m_testCase.reset();
        m_testCase.m_name = atts.value("name");
        emit m_runner->testCaseChanged(m_testCase);

        qCDebug(LogQtTestRunnerCore, "TestCase %s started", m_testCase.m_name.toStdString().c_str());
        m_inTestCase = true;
        return true;
    }

    if (!m_inEnvironment && qName == "Environment")
    {
        m_inEnvironment = true;
        return true;
    }

    if (m_inEnvironment)
    {
        // ignore all
        return true;
    }

    if (!m_inTestFunction && qName == "TestFunction")
    {
        m_testCase.m_testfunctions.append(TestFunction());
        TestFunction &testfunction = m_testCase.m_testfunctions.last();
        testfunction.m_name = atts.value("name");
        testfunction.m_casename = m_testCase.m_name;
        emit m_runner->testCaseChanged(m_testCase);

        qCDebug(LogQtTestRunnerCore, "TestCase %s started", testfunction.m_name.toStdString().c_str());
        m_inTestFunction = true;
        return true;
    }

    if (!m_inIncident && qName == "Incident")
    {
        TestFunction &testfunction = m_testCase.m_testfunctions.last();
        testfunction.m_incidents.append(Incident());
        Incident &incident = testfunction.m_incidents.last();
        incident.m_type = atts.value("type");
        incident.m_file = atts.value("file");
        incident.m_line = atts.value("line");
        emit m_runner->testCaseChanged(m_testCase);

        m_inIncident = true;
        return true;
    }

    if (!m_inDuration && qName == "Duration")
    {
        if (m_inTestFunction)
        {
            TestFunction &testfunction = m_testCase.m_testfunctions.last();
            testfunction.m_duration = atts.value("msecs").toDouble();
        }
        else if (m_inTestCase)
        {
            m_testCase.m_duration = atts.value("msecs").toDouble();
        }
        emit m_runner->testCaseChanged(m_testCase);

        m_inDuration = true;
        return true;
    }

    if (!m_inMessage && qName == "Message")
    {
        TestFunction &testfunction = m_testCase.m_testfunctions.last();
        testfunction.m_messages.append(Message());
        Message &message = testfunction.m_messages.last();
        message.m_type = atts.value("type");
        message.m_file = atts.value("file");
        message.m_line = atts.value("line");
        emit m_runner->testCaseChanged(m_testCase);

        m_inMessage = true;
        return true;
    }

    if (!m_inDescription && qName == "Description")
    {
        m_inDescription = true;
        return true;
    }

    if (!m_inDataTag && qName == "DataTag")
    {
        m_inDataTag = true;
        return true;
    }

    if (!m_inBenchmarkResult && qName == "BenchmarkResult")
    {
        m_inBenchmarkResult = true;
        return true;
    }

    return true;
}

/******************************************************************************/
bool UnitTestOutputHandler::endElement(const QString & namespaceURI,
                                       const QString & localName,
                                       const QString & qName)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if (m_inTestCase && qName == "TestCase")
    {
        m_testCase.m_done = true;
        emit m_runner->testCaseChanged(m_testCase);
        emit m_runner->endTestCase(m_testCase);

        qCDebug(LogQtTestRunnerCore, "TestCase %s finished", m_testCase.m_name.toStdString().c_str());
        m_inTestCase = false;
        return true;
    }

    if (m_inEnvironment && qName == "Environment")
    {
        m_inEnvironment = false;
        return true;
    }

    if (m_inEnvironment)
    {
        // ignore all;
        return true;
    }

    if (m_inTestFunction && qName == "TestFunction")
    {
        TestFunction &testfunction = m_testCase.m_testfunctions.last();
        testfunction.m_done = true;
        emit m_runner->testCaseChanged(m_testCase);
        emit m_runner->endTestFunction(testfunction);

        qCDebug(LogQtTestRunnerCore, "TestCase %s finished", testfunction.m_name.toStdString().c_str());
        m_inTestFunction = false;
        return true;
    }

    if (m_inIncident && qName == "Incident")
    {
        Incident &incident = m_testCase.m_testfunctions.last().m_incidents.last();
        incident.m_done = true;
        emit m_runner->testCaseChanged(m_testCase);

        m_inIncident = false;
        return true;
    }

    if (m_inDuration && qName == "Duration")
    {
        m_inDuration = false;
        return true;
    }

    if (m_inDescription && qName == "Description")
    {
        m_inDescription = false;
        return true;
    }

    if (m_inMessage && qName == "Message")
    {
        Message &message = m_testCase.m_testfunctions.last().m_messages.last();
        message.m_done = true;
        emit m_runner->testCaseChanged(m_testCase);

        m_inMessage = false;
        return true;
    }

    if (m_inDataTag && qName == "DataTag")
    {
        m_inDataTag = false;
        return true;
    }

    if (m_inBenchmarkResult && qName == "BenchmarkResult")
    {
        m_inBenchmarkResult = false;
        return true;
    }

    return true;
}

/******************************************************************************/
bool UnitTestOutputHandler::characters(const QString & ch)
{
    QString txt = ch.trimmed();

    if (m_inEnvironment)
    {
        return true;
    }

    if (!txt.isEmpty())
    {
        if (m_inMessage)
        {
            Message &message = m_testCase.m_testfunctions.last().m_messages.last();
            message.m_description = txt;
            return true;
        }
        if (m_inIncident)
        {
            Incident &incident = m_testCase.m_testfunctions.last().m_incidents.last();
            if (m_inDescription)
                incident.m_description = txt;
            if (m_inDataTag)
                incident.m_datatag = txt;

            return true;
        }
    }
    return true;
}

/******************************************************************************/
