#ifndef CONSOLE_RUNNER_H
#define CONSOLE_RUNNER_H

#include <QObject>

#include "testsettings.h"
#include "unittestoutputhandler.h"

class TestManager; //forward

/******************************************************************************/
/** \brief Class that uses the testmanager to run tests and report them in a console */
class ConsoleRunner : public QObject
{
    Q_OBJECT
public:
    /** \brief Create a console runner that uses a testmanager and testsettings */
    explicit ConsoleRunner(TestManager *a_testManager,
                           TestSettings *a_testSettings);
    virtual ~ConsoleRunner();

public slots:
    /** \brief Start the test runner */
    void onRun();

signals:
    /** \brief Test runner finished */
    void testingFinished();

private slots:
    /** \brief Report a new testsuite */
    void onFoundTestSuite(const QString &a_testSuiteName, unsigned int a_nrTests);

    /** \brief Report end test function */
    void onEndTestFunction(const TestFunction &a_testFunction);

    /** \brief Report crash test suite */
    void onCrashTestSuite(const TestSuite &a_testSuite);

    /** \brief Report testing finished */
    void onFinishedTesting();

private:
    void startCollecting();

    TestManager *m_testManager;
    TestSettings *m_testSettings;

    unsigned int m_totalNrTestsFound;
    unsigned int m_totalNrTestsRun;
    unsigned int m_totalNrTestsPassed;
    bool m_allTestsOk;
};

/******************************************************************************/

#endif // CONSOLE_RUNNER_H
