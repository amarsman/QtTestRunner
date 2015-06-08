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
    QString m_testunit;
    QString m_testcase;
    QString m_testname;
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
    void unitTestFound(const QString &findResult);

    void testSuiteChanged(const TestSuite    &a_testSuite);
    void endTestSuite    (const TestSuite    &a_testSuite);
    void endTestCase     (const TestCase     &a_testCase);
    void endTestFunction (const TestFunction &a_testFunction);

    void finished();

protected:
    void run(void);

private slots:
    void onTestSuiteChanged(const TestSuite &a_testSuite);
    void onEndTestSuite(const TestSuite &a_testSuite);
    void onEndTestCase(const TestCase &a_testCase);
    void onEndTestFunction(const TestFunction &a_testFunction);

private:
    bool isUnitTest(const QString &filename);
    QList<TestTriple> getTests(const QString &filename);

    TestSettings *m_settings;
    bool m_stopRequested;
    bool m_running;
    QSharedPointer<QSemaphore> sem;
};

/******************************************************************************/

#endif // TEST_MANAGER_H

