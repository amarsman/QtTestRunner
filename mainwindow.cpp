#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logging.h"

/******************************************************************************/
MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent)
 , ui(new Ui::MainWindow)
{
    qCDebug(LogQtTestRunner);
    ui->setupUi(this);
}

/******************************************************************************/
MainWindow::~MainWindow()
{
    qCDebug(LogQtTestRunner);
    delete ui;
}

/******************************************************************************/
