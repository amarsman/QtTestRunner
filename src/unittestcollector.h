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
class UnitTestCollector : public QObject, public QRunnable
{
    Q_OBJECT
public:

    UnitTestCollector();
    ~UnitTestCollector();

    void start(const TestSettings &settings);
    void stop();

signals:
    void unittestFound(const QString &findResult);
    void collectionFinished();
    void unitTestResult(int jobnr, const QString &testResult, bool ok);

protected:
    void run(void);

private slots:
    void onUnitTestResult(int jobnr, const QString &testResult, bool ok);

private:
    bool isUnitTest(const QString &filename);

    bool m_stopRequested;
    bool m_running;
    QSharedPointer<QSemaphore> sem;

    TestSettings m_settings;
};

/******************************************************************************/

#endif // UNIT_TEST_FINDER_H

