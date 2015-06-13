#ifndef TEST_MANAGER_H
#define TEST_MANAGER_H
#include <QObject>
#include <QRunnable>
#include <QString>
#include <QStringList>
#include <QSemaphore>
#include <QSharedPointer>
#include "testsettings.h"
#include "unittestoutputhandler.h"

/** \brief Struct that holds a testable */
class TestTriple
{
public:
    TestTriple()
    {
        reset();
    }
    /** \brief reset data */
    void reset()
    {
        m_unitTestName = "";
        m_testCaseName = "";
        m_testName = "";
    }

    QString m_unitTestName;    /*!< \brief Name of test suite */
    QString m_testCaseName;    /*!< \brief Name of test case */
    QString m_testName;        /*!< \brief Name of test function */
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
    void start(TestSettings *a_settings);

    /** \brief Stop the test manager */
    void stop();

signals:
    /** \brief Report a new unittest */
    void unitTestFound(const QString &findResult, unsigned int a_nrTests);

    /** \brief Report test function done */
    void endTestFunction (const TestFunction &a_testFunction);

    /** \brief Report test suite crashed */
    void crashTestSuite  (const TestSuite    &a_testSuiteName);

    /** \brief Report testing finished */
    void testingFinished();

private slots:
    void onEndTestFunction(const TestFunction &a_testFunction);
    void onCrashTestSuite(const TestSuite &a_testSuiteName);

private:
    void run(void);
    bool isUnitTest(const QString &filename);
    void getTests(const QString &a_unittest, QList<TestTriple> &tests, unsigned int &nrtests);

    TestSettings *m_settings;
    bool m_stopRequested;
    bool m_running;
    QSharedPointer<QSemaphore> sem;
};

/******************************************************************************/

#endif // TEST_MANAGER_H

