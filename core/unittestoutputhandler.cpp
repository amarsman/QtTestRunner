#include <QFileInfo>
#include "unittestoutputhandler.h"

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
    qRegisterMetaType<TestFunctionResult>("TestFunctionResult");
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
        m_testCaseResult.testcasename = atts.value("name");
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
        m_testFunctionResult.testcasename = m_testCaseResult.testcasename;
        m_testFunctionResult.testfunctionname = atts.value("name");
        m_testFunctionResult.busy = true;
        emit m_runner->unitTestResult(m_testFunctionResult);
        m_inTestFunction = true;
        return true;
    }

    if (!m_inIncident && qName == "Incident")
    {
        QString filepath = QFileInfo(m_testpath + atts.value("file")).absoluteFilePath();
        m_testFunctionResult.type = atts.value("type");
        m_testFunctionResult.file = atts.value("file");
        m_testFunctionResult.line = atts.value("line");
        emit m_runner->unitTestResult(m_testFunctionResult);

        m_inIncident = true;
        return true;
    }

    if (!m_inDuration && qName == "Duration")
    {
        if (m_inTestFunction)
        {
            m_testFunctionResult.duration = atts.value("msecs").toDouble();
            emit m_runner->unitTestResult(m_testFunctionResult);
        }
        else if (m_inTestCase)
        {
            m_testCaseResult.duration = atts.value("msecs").toDouble();
        }
        m_inDuration = true;
        return true;
    }

    if (!m_inMessage && qName == "Message")
    {
        m_inMessage = true;
        m_testMessage.msg_class = atts.value("type");
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

    fprintf(stderr,"Elem: %s\n", qName.toStdString().c_str());

    for (int i=0; i<atts.count(); i++)
    {
        fprintf(stderr,"Attr: %s = %s\n", atts.qName(i).toStdString().c_str(),
                atts.value(i).toStdString().c_str());
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
        m_inTestFunction = false;
        m_testFunctionResult.busy = false;
        emit m_runner->unitTestResult(m_testFunctionResult);
        m_testFunctionResult.reset();
        return true;
    }

    if (m_inIncident && qName == "Incident")
    {
        if (!m_testMessage.empty)
        {
            m_testFunctionResult.incidents.append(m_testMessage);
            emit m_runner->unitTestResult(m_testFunctionResult);
            m_testMessage.reset();
        }
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
        if (!m_testMessage.empty)
        {
            m_testFunctionResult.messages.append(m_testMessage);
            emit m_runner->unitTestResult(m_testFunctionResult);
            m_testMessage.reset();
        }
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

    fprintf(stderr,"/Elem: %s\n", qName.toStdString().c_str());

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
        if ((m_inMessage && m_inDescription) ||
                (m_inIncident && m_inDescription))
        {
            m_testMessage.message = txt;
            m_testMessage.empty = false;
            return true;
        }
        //fprintf(stderr,"Characters: %s\n", ch.toStdString().c_str());
    }
    return true;
}

/******************************************************************************/
