#ifndef MYXMLCONTENTHANDLER_H
#define MYXMLCONTENTHANDLER_H

#include <QString>
#include <QObject>
#include <QRegularExpression>
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
class TestSuite
{
public:
    TestSuite() { reset(); }
    void reset() { m_testCases.clear(); }

    QList<TestCase> m_testCases;
};

/******************************************************************************/
class UnitTestOutputHandler : public QObject
{
    Q_OBJECT
public:
    UnitTestOutputHandler(UnitTestRunner *a_handler, QString a_testpath);
    virtual ~UnitTestOutputHandler();
    void processXmlLine(const QString &line);

private:
    UnitTestRunner *m_runner;
    TestSuite m_testSuite;

    TestCase *m_testCase;
    TestFunction *m_testFunction;
    Incident *m_incident;

    QRegularExpression re_xml;
    QRegularExpression re_tc_start;
    QRegularExpression re_tc_end;
    QRegularExpression re_environment_start;
    QRegularExpression re_environment_end;
    QRegularExpression re_tf_start;
    QRegularExpression re_tf_end;
    QRegularExpression re_incident;
    QRegularExpression re_incident_start;
    QRegularExpression re_incident_end;
    QRegularExpression re_message_start;
    QRegularExpression re_message_end;
    QRegularExpression re_duration;
    QRegularExpression re_description;
    QRegularExpression re_description_start;
    QRegularExpression re_description_end;
    QRegularExpression re_datatag;
    QRegularExpression re_benchmark;

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
