#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include "unittestcollector.h"
#include "testsettings.h"
#include "unittestmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const TestSettings &a_settings, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onStartClicked();
    void onUnitTestResult(int jobnr,
                          const QString &testCase,
                          const QString &testFunction,
                          const QString &testResult);
    void onFinished();

private:
    Ui::MainWindow *ui;
    QScopedPointer<UnitTestCollector> m_unitTestCollector;

    TestSettings m_settings;
    UnitTestModel m_unittestmodel;
};

#endif // MAINWINDOW_H
