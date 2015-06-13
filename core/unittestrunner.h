#ifndef UNIT_TEST_RUNNER_H
#define UNIT_TEST_RUNNER_H
#include <QObject>
#include <QRunnable>
#include <QString>
#include <QSemaphore>
#include <QSharedPointer>
#include <QRegularExpression>
#include <QMutex>

// forward
class TestSuite;
class TestCase;
class TestFunction;

/******************************************************************************/
/** \brief Runnable that runs a single unittest, testcase or test function
 * and reports results by signals */
class UnitTestRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:
    /** \brief Create a test runner that clears a semaphore when done */
    UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore);
    ~UnitTestRunner();

    /** \brief Start a test */
    bool start(int jobnr,
               const QString &a_unitTestName,
               const QString &a_testCaseName,
               const QString &a_testName);

    /** \brief Stop*/
    bool stop();

signals:
    /** \brief Report test function done */
    void endTestFunction (const TestFunction &a_testFunction);

    /** \brief Report test suite crashed */
    void crashTestSuite  (const TestSuite    &a_testSuite);

private:
    void run(void);

    QString m_unitTestName;
    QString m_testCaseName;
    QString m_testName;
    QSharedPointer<QSemaphore> m_semaphore;
    bool m_running;
    bool m_stopRequested;
    int m_jobnr;
};

/******************************************************************************/

#endif // UNIT_TEST_RUNNER_H

