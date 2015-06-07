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
class UnitTestRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:

    UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore);
    ~UnitTestRunner();

    bool start(int jobnr,
               const QString &a_unitTest,
               const QString &a_testName);
    bool stop();

signals:
    void testSuiteChanged(const TestSuite    &a_testSuite);
    void endTestSuite    (const TestSuite    &a_testSuite);
    void endTestCase     (const TestCase     &a_testCase);
    void endTestFunction (const TestFunction &a_testFunction);

protected:
    void run(void);

private:
    QString m_unitTest;
    QString m_testName;
    QSharedPointer<QSemaphore> m_semaphore;
    bool m_running;
    bool m_stopRequested;
    int m_jobnr;
};

/******************************************************************************/

#endif // UNIT_TEST_RUNNER_H

