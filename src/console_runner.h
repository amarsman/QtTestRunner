#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QScopedPointer>
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
                           TestSettings *a_settings);
    virtual ~ConsoleRunner();

public slots:
    /** \brief Start the test runner */
    void onRun();

signals:
    /** \brief Test runner finished */
    void testingFinished();

private slots:
    /** \brief Report a new unittest */
    void onUnitTestFound(const QString &a_path, unsigned int a_nrTests);

    /** \brief Report testing finished */
    void onTestingFinished();

    /** \brief Report end test case */
    void onEndTestCase(const TestCase &result);

    /** \brief Report end test function */
    void onEndTestFunction(const TestFunction &result);

    /** \brief Report crash test suite */
    void onCrashTestSuite(const TestSuite &a_testSuiteName);

private:
    void startCollecting();

    TestManager *m_testManager;
    TestSettings *m_settings;

    unsigned int m_totalNrTestsFound;
    unsigned int m_totalNrTestsRun;
    bool m_allTestsOk;
};

/******************************************************************************/

#endif // MAIN_TASK_H
