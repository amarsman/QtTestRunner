#ifndef MYXMLCONTENTHANDLER_H
#define MYXMLCONTENTHANDLER_H

#include <QString>
#include <QXmlDefaultHandler>
#include <QXmlAttributes>
#include "unittestrunner.h"

/******************************************************************************/
class TestCaseResult
{
public:
    TestCaseResult()
        : testcasename("") { }
    QString testcasename;
    double duration;

    void print()
    {
        fprintf(stderr, "%-40s %12.3lf\n",
                testcasename.toStdString().c_str(),
                duration);
    }};

/******************************************************************************/
class TestMessage
{
public:
    TestMessage()
      : msg_class("")
      , message("")
      , empty(true) {}

    void reset()
    {
        msg_class = "";
        message = "";
        empty = true;
    }

    QString msg_class;
    QString message;
    bool empty;
};

/******************************************************************************/
class TestFunctionResult
{
public:
    TestFunctionResult()
        : testcasename("")
        , testfunctionname("")
        , type("")
        , file("")
        , line("")
        , busy(false)
        , duration(0)
    { }

    void reset()
    {
        testcasename = "";
        testfunctionname = "";
        type = "";
        file = "";
        line = "";
        busy = false;
        duration = 0;
        messages.clear();
        incidents.clear();
    };
    QString testcasename;
    QString testfunctionname;
    QString type;
    QString file;
    QString line;
    bool busy;
    double duration;
    QList<TestMessage> messages;
    QList<TestMessage> incidents;

    void print() const
    {
        fprintf(stderr, "testcase=%s\n"
                        "function=%s\n"
                        "type=%s\n"
                        "file=%s\n"
                        "line=%s\n"
                        "duration=%g\n"
                        "busy=%d\n\n",
                testcasename.toStdString().c_str(),
                testfunctionname.toStdString().c_str(),
                type.toStdString().c_str(),
                file.toStdString().c_str(),
                line.toStdString().c_str(),
                duration,
                busy);

        for (auto it = messages.begin(); it != messages.end(); ++it)
        {
            fprintf(stderr, "%s %s", it->msg_class.toStdString().c_str(), it->message.toStdString().c_str());
        }
        for (auto it = incidents.begin(); it != incidents.end(); ++it)
        {
            fprintf(stderr, "%s %s", it->msg_class.toStdString().c_str(), it->message.toStdString().c_str());
        }
    }
};

class UnitTestRunner; //forward

/******************************************************************************/
class UnitTestOutputHandler : public QObject, public QXmlDefaultHandler
{
    Q_OBJECT
public:
    UnitTestOutputHandler(UnitTestRunner *a_handler, QString a_testpath);
    virtual ~UnitTestOutputHandler();

protected:
    bool startElement(const QString & namespaceURI,
                      const QString & localName,
                      const QString & qName,
                      const QXmlAttributes & atts);
    bool endElement(const QString & namespaceURI,
                    const QString & localName,
                    const QString & qName);

    bool characters(const QString & ch);

private:
    UnitTestRunner *m_runner;
    TestCaseResult     m_testCaseResult;
    TestFunctionResult m_testFunctionResult;
    TestMessage m_testMessage;
    bool m_inTestCase;
    bool m_inEnvironment;
    bool m_inTestFunction;
    bool m_inIncident;
    bool m_inDuration;
    bool m_inMessage;
    bool m_inDescription;
    bool m_inDataTag;
    bool m_inBenchmarkResult;
    QString m_testpath;
};


#endif // MYXMLCONTENTHANDLER_H
