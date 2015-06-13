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

class TestTriple
{
public:
    TestTriple()
    {
        reset();
    }
    void reset()
    {
        m_unitTestName = "";
        m_testCaseName = "";
        m_testName = "";
    }

    QString m_unitTestName;
    QString m_testCaseName;
    QString m_testName;
};

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
    void unitTestFound(const QString &findResult, unsigned int a_nrTests);

    void endTestFunction (const TestFunction &a_testFunction);
    void crashTestSuite  (const TestSuite    &a_testSuiteName);

    void testingFinished();

protected:
    void run(void);

private slots:
    void onEndTestFunction(const TestFunction &a_testFunction);
    void onCrashTestSuite(const TestSuite &a_testSuiteName);

private:
    bool isUnitTest(const QString &filename);
    void getTests(const QString &a_unittest, QList<TestTriple> &tests, unsigned int &nrtests);

    TestSettings *m_settings;
    bool m_stopRequested;
    bool m_running;
    QSharedPointer<QSemaphore> sem;
};

/******************************************************************************/

#endif // TEST_MANAGER_H

