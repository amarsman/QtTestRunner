#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QScopedPointer>
#include "testsettings.h"

class UnitTestCollector; //forward

/******************************************************************************/
class MainTask : public QObject
{
    Q_OBJECT
public:
    explicit MainTask(QObject *parent, const TestSettings &a_settings);
    virtual ~MainTask();

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

    QScopedPointer<UnitTestCollector> m_unitTestCollector;
    TestSettings m_settings;
};

#endif // MAIN_TASK_H
