#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QStandardItemModel>
#include <QStandardItem>

#include "testmanager.h"
#include "testsettings.h"
#include "unittestoutputhandler.h"

namespace Ui {
class MainWindow;
}

class GuiRunner : public QMainWindow
{
    Q_OBJECT

public:
    GuiRunner(TestManager *a_testManager,
              TestSettings *a_settings);
    ~GuiRunner();

private slots:
    void onStartClicked();
    void onUnitTestResult(const TestFunctionResult &result);
    void onFinished();

private:
    Ui::MainWindow *ui;
    TestManager *m_testManager;
    TestSettings *m_settings;
    QStandardItemModel m_unittestmodel;
};

#endif // MAINWINDOW_H
