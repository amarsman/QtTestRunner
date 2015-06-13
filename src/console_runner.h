#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QScopedPointer>
#include "testsettings.h"
#include "unittestoutputhandler.h"

class TestManager; //forward

/******************************************************************************/
class ConsoleRunner : public QObject
{
    Q_OBJECT
public:
    explicit ConsoleRunner(TestManager *a_testManager,
                           TestSettings *a_settings);
    virtual ~ConsoleRunner();

public slots:
    void onRun();
    void onUnitTestFound(const QString &a_path, unsigned int a_nrTests);
    void onTestingFinished();

signals:
    void testingFinished();

private slots:
    void onEndTestCase(const TestCase &result);
    void onEndTestFunction(const TestFunction &result);
    void onCrashTestSuite(const TestSuite &a_testSuiteName);

private:
    void startCollecting();

    TestManager *m_testManager;
    TestSettings *m_settings;

    unsigned int m_totalNrTestsFound;
    unsigned int m_totalNrTestsRun;
    bool m_allTestsOk;
};

/******************************************************************************/

#endif // MAIN_TASK_H
