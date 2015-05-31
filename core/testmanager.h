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
    void unitTestResult(const TestCase &result);

    void endTestCase(const TestCase &result);
    void endTestFunction(const TestFunction &result);

protected:
    void run(void);

private slots:
    void onTestCaseChanged(const TestCase &result);
    void onEndTestFunction(const TestFunction &result);
    void onEndTestCase(const TestCase &result);

private:
    bool isUnitTest(const QString &filename);

    TestSettings *m_settings;
    bool m_stopRequested;
    bool m_running;
    QSharedPointer<QSemaphore> sem;
};

/******************************************************************************/

#endif // TEST_MANAGER_H

