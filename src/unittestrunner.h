#ifndef UNIT_TEST_FINDER_H
#define UNIT_TEST_FINDER_H
#include <QObject>
#include <QRunnable>
#include <QString>
#include <QSemaphore>
#include <QSharedPointer>

/******************************************************************************/
class UnitTestRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:

    UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore);
    ~UnitTestRunner();

    void start(int jobnr, const QString &a_unitTest);
    void stop();

signals:
    void unitTestResult(int jobnr, const QString &testResult, bool ok);

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

#endif // UNIT_TEST_FINDER_H

