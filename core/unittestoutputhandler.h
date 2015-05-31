#ifndef MYXMLCONTENTHANDLER_H
#define MYXMLCONTENTHANDLER_H

#include <QString>
#include <QXmlDefaultHandler>
#include <QXmlAttributes>
#include "unittestrunner.h"

class UnitTestRunner; //forward

/******************************************************************************/
class Message
{
public:
    Message() { reset(); }
    void reset() { m_type=""; m_file=""; m_line="";  m_description=""; m_done=false; }

    QString m_type;
    QString m_file;
    QString m_line;
    QString m_description;
    bool    m_done;
};

/******************************************************************************/
class Incident
{
public:
    Incident() { reset(); }
    void reset() { m_type=""; m_file=""; m_line="";  m_datatag=""; m_description=""; m_done=false; }

    QString m_type;
    QString m_file;
    QString m_line;
    QString m_datatag;
    QString m_description;
    bool    m_done;
};

/******************************************************************************/
class TestFunction
{
public:
    TestFunction() { reset(); }
    void reset() { m_name=""; m_messages.clear(); m_incidents.clear(); m_duration=0; m_done=false; }

    QString         m_casename;
    QString         m_name;
    QList<Message>  m_messages;
    QList<Incident> m_incidents;
    // todo         m_benchmarks
    double          m_duration;
    bool            m_done;
};

/******************************************************************************/
class TestCase
{
public:
    TestCase() { reset(); }
    void reset() { m_name=""; m_testfunctions.clear(); m_duration=0; m_done=false; }

    QString             m_name;
    // todo             m_environment
    QList<TestFunction> m_testfunctions;
    double              m_duration;
    bool                m_done;
};


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
    TestCase        m_testCase;

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
