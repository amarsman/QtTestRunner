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
    void onUnitTestFound(const QString &a_path, int a_nrtests);
    void onCollectionFinished();

signals:
    void finished();

private slots:
    void onEndTestCase(const TestCase &result);
    void onEndTestFunction(const TestFunction &result);

private:
    void startCollecting();

    TestManager *m_testManager;
    TestSettings *m_settings;
};

/******************************************************************************/

#endif // MAIN_TASK_H
