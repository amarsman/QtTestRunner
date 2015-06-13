#ifndef TEST_MANAGER_H
#define TEST_MANAGER_H

#include <QObject>
#include <QRunnable>
#include <QSemaphore>
#include <QSharedPointer>
#include <QString>

#include "testsettings.h"
#include "unittestoutputhandler.h"

/******************************************************************************/
/** \brief Struct that holds a testable */
class UnitTestTriple
{
public:
    UnitTestTriple()
    {
        reset();
    }
    /** \brief reset data */
    void reset()
    {
        m_testSuiteName = "";
        m_testCaseName = "";
        m_testFunctionName = "";
    }

    QString m_testSuiteName;     /*!< \brief Name of test suite */
    QString m_testCaseName;      /*!< \brief Name of test case */
    QString m_testFunctionName;  /*!< \brief Name of test function */
};

/******************************************************************************/
/** \brief Runnable that collects testables and runs them multi threaded */
class TestManager : public QObject, public QRunnable
{
    Q_OBJECT
public:

    TestManager();
    ~TestManager();

public slots:
    /** \brief Start the test manager */
    void start(TestSettings *a_testSettings);

    /** \brief Stop the test manager */
    void stop();

signals:
    /** \brief Report a new test suite */
    void foundTestSuite(const QString &a_testSuiteName, unsigned int a_nrTests);

    /** \brief Report test function done */
    void endTestFunction(const TestFunction &a_testFunction);

    /** \brief Report test suite crashed */
    void crashTestSuite(const TestSuite &a_testSuiteName);

    /** \brief Report testing finished */
    void finishedTesting();

private:
    void run(void);
    bool isTestSuite(const QString &a_fileName);
    void getTests(const QString &a_testSuiteName,
                  QList<UnitTestTriple> &a_testTriples,
                  unsigned int &a_nrTests);

    TestSettings *m_testSettings;
    bool m_stopRequested;
    bool m_running;
    QSharedPointer<QSemaphore> sem;
};

/******************************************************************************/

#endif // TEST_MANAGER_H

