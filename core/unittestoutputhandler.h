#ifndef UNIT_TEST_OUTPUT_HANDLER_H
#define UNIT_TEST_OUTPUT_HANDLER_H

#include <QList>
#include <QMutex>
#include <QObject>
#include <QRegularExpression>
#include <QString>

class UnitTestRunner; //forward

/******************************************************************************/
/** \brief Helper class that locks a mutex during its lifetime */
class Locker
{
public:
    /** \brief Create a locker object for a mutex */
    Locker(QMutex &a_lock) : m_lock(a_lock)
    {
        m_lock.lock();
    }
    ~Locker()
    {
        m_lock.unlock();
    }
private:
    QMutex &m_lock;   /*!< \brief Associated mutex */
};

/******************************************************************************/
/** \brief Struct that holds a benchmark result */
class Benchmark
{
public:
    Benchmark()
    {
        reset();
    }

    /** \brief Reset data */
    void reset()
    {
        m_metric="";
        m_tag="";
        m_value="";
        m_iterations="";
        m_done=false;
    }

    QString m_metric;         /*!< \brief Units of metric */
    QString m_tag;            /*!< \brief Data tag for benchmark */
    QString m_value;          /*!< \brief Benchmark metric */
    QString m_iterations;     /*!< \brief Number of iterations */
    bool    m_done;           /*!< \brief Benchmark done */
};

/******************************************************************************/
/** \brief Struct that holds a message result */
class Message
{
public:
    Message()
    {
        reset();
    }

    /** \brief Reset data */
    void reset()
    {
        m_type="";
        m_file="";
        m_line="";
        m_description="";
        m_done=false;
    }

    QString m_type;         /*!< \brief Type of message */
    QString m_file;         /*!< \brief File associated with message */
    QString m_line;         /*!< \brief Line associated with message */
    QString m_description;  /*!< \brief Message content */
    bool    m_done;         /*!< \brief Message done */
};

/******************************************************************************/
/** \brief Struct that holds an incident result */
class Incident
{
public:
    Incident()
    {
        reset();
    }

    /** \brief Reset data */
    void reset()
    {
        m_type="";
        m_file="";
        m_line="";
        m_datatag="";
        m_description="";
        m_done=false;
    }

    QString m_type;         /*!< \brief Type of incident */
    QString m_file;         /*!< \brief File associated with incident */
    QString m_line;         /*!< \brief Line associated with incident */
    QString m_datatag;      /*!< \brief Datatag associated with incident */
    QString m_description;  /*!< \brief Incident content */
    bool    m_done;         /*!< \brief Incident done */
};

/******************************************************************************/
/** \brief Struct that holds an test function result */
class TestFunction
{
public:
    TestFunction()
    {
        reset();
    }

    /** \brief Reset data */
    void reset()
    {
        m_name = "";
        m_messages.clear();
        m_incidents.clear();
        m_duration = "";
        m_done = false;
        m_pass = true;
    }

    QString          m_casename;          /*!< \brief Name of test case */
    QString          m_name;              /*!< \brief Name of test function */
    QList<Message>   m_messages;          /*!< \brief Messages in test function */
    QList<Incident>  m_incidents;         /*!< \brief Incidents in test function */
    QList<Benchmark> m_benchmarks;        /*!< \brief Benchmarks in test function */
    QString          m_duration;          /*!< \brief Duration of test function */
    bool             m_done;              /*!< \brief Test function done */
    bool             m_pass;              /*!< \brief Test function passed */
};

/******************************************************************************/
/** \brief Struct that holds an test case result */
class TestCase
{
public:
    TestCase()
    {
        reset();
    }

    /** \brief Reset data */
    void reset()
    {
        m_name = "";
        m_testfunctions.clear();
        m_duration = "";
        m_done = false;
        m_pass = true;
    }

    QString             m_name;            /*!< \brief Name of test case */
    // todo             m_environment
    QList<TestFunction> m_testfunctions;   /*!< \brief Test functions in test case */
    QString             m_duration;        /*!< \brief Duration of test case */
    bool                m_done;            /*!< \brief Test case done */
    bool                m_pass;            /*!< \brief Test case passed */
};


/******************************************************************************/
/** \brief Struct that holds an test suite result */
class TestSuite
{
public:
    TestSuite()
    {
        reset();
    }

    /** \brief Reset data */
    void reset()
    {
        m_name = "";
        m_testCases.clear();
        m_done = false;
        m_pass = true;
    }

    QString         m_name;           /*!< \brief Name of test suite */
    QList<TestCase> m_testCases;      /*!< \brief Test cases in test suite*/
    bool            m_done;           /*!< \brief Test suite done */
    bool            m_pass;           /*!< \brief Test suite passed */
};

/******************************************************************************/
/** \brief Class that parses xml test results and puts these in a data tree */
class UnitTestOutputHandler : public QObject
{
    Q_OBJECT
public:
    /** \brief Create an output handler for a testsuite */
    UnitTestOutputHandler(const QString &a_testSuiteName);
    virtual ~UnitTestOutputHandler();

    /** \brief Set the test runner for reporting back */
    void setUnitTestRunner(UnitTestRunner *a_unitTestRunner);

    /** \brief Process an xml line */
    void processXmlLine(const QString &a_line);

    /** \brief The collected data tree */
    const TestSuite &getTestSuite() const;

private:
    void propagateResults();
    bool hasTestFunctionPassed();

    QScopedPointer<TestSuite> m_testSuite;
    UnitTestRunner *m_unitTestRunner;

    TestCase *m_testCase;
    TestFunction *m_testFunction;
    Incident *m_incident;
    Message *m_message;
    Benchmark *m_benchmark;

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
};

extern QMutex g_access;

/******************************************************************************/

#endif // UNIT_TEST_OUTPUT_HANDLER_H
