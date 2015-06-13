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
               const QString &a_unitTestName,
               const QString &a_testCaseName,
               const QString &a_testName);
    bool stop();

signals:
    void endTestFunction (const TestFunction &a_testFunction);
    void crashTestSuite  (const TestSuite    &a_testSuite);

protected:
    void run(void);

private:
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

