#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QStandardItemModel>
#include <QStandardItem>


#include "testmanager.h"
#include "testsettings.h"
#include "unittestoutputhandler.h"
#include "unittestmodel.h"

/******************************************************************************/
namespace Ui {
class MainWindow;
}

/******************************************************************************/
class GuiRunner : public QMainWindow
{
    Q_OBJECT

public:
    GuiRunner(TestManager *a_testManager,
              TestSettings *a_settings);
    ~GuiRunner();

private slots:
    void onStartClicked();
    void onTestSuiteChanged(const TestSuite &a_testSuite);
    void onRefreshDone();
    void onFinished();

private:
    Ui::MainWindow *ui;
    TestManager *m_testManager;
    TestSettings *m_settings;
    UnitTestModel m_unittestmodel;
};

/******************************************************************************/

#endif // MAINWINDOW_H
