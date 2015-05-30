#ifndef UNIT_TEST_RUNNER_H
#define UNIT_TEST_RUNNER_H
#include <QObject>
#include <QRunnable>
#include <QString>
#include <QStringList>
#include <QSemaphore>
#include <QSharedPointer>
#include "testsettings.h"

/******************************************************************************/
class TestManager : public QObject, public QRunnable
{
    Q_OBJECT
public:

    TestManager();
    ~TestManager();

public slots:
    void start(TestSettings *a_settings);
    void stop();

signals:
    void unitTestFound(const QString &findResult);
    void finished();
    void unitTestResult(int jobnr,
                        const QString &testCase,
                        const QString &testFunction,
                        const QString &testResult);

protected:
    void run(void);

private slots:
    void onUnitTestResult(int jobnr,
                          const QString &testCase,
                          const QString &testFunction,
                          const QString &testResult);

private:
    bool isUnitTest(const QString &filename);

    TestSettings *m_settings;
    bool m_stopRequested;
    bool m_running;
    QSharedPointer<QSemaphore> sem;
};

/******************************************************************************/

#endif // UNIT_TEST_FINDER_H

