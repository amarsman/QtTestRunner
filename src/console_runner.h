#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QScopedPointer>
#include "testsettings.h"

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
    void onUnitTestFound(const QString &a_path);
    void onCollectionFinished();

signals:
    void finished();

private slots:
    void onUnitTestResult(int jobnr,
                          const QString &testCase,
                          const QString &testFunction,
                          const QString &testResult);

private:
    void startCollecting();

    TestManager *m_testManager;
    TestSettings *m_settings;
};

#endif // MAIN_TASK_H
