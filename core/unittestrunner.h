#ifndef UNIT_TEST_RUNNER_H
#define UNIT_TEST_RUNNER_H
#include <QObject>
#include <QRunnable>
#include <QString>
#include <QSemaphore>
#include <QSharedPointer>
#include <QRegularExpression>

class TestCase; // forward
class TestFunction;

/******************************************************************************/
class UnitTestRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:

    UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore);
    ~UnitTestRunner();

    bool start(int jobnr, const QString &a_unitTest);
    bool stop();

signals:
    void testCaseChanged(const TestCase &result);
    void endTestCase(const TestCase &result);
    void endTestFunction(const TestFunction &result);

protected:
    void run(void);

private:
    QString m_unitTest;
    QSharedPointer<QSemaphore> m_semaphore;
    bool m_running;
    bool m_stopRequested;
    int m_jobnr;
};

/******************************************************************************/

#endif // UNIT_TEST_RUNNER_H

