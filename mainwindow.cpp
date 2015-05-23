#include "mainwindow.h"
#include "ui_mainwindow.h"

/******************************************************************************/
MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent)
 , ui(new Ui::MainWindow)
 , category("MainWindow")
{
    qCDebug(category);
    ui->setupUi(this);
}

/******************************************************************************/
MainWindow::~MainWindow()
{
    qCDebug(category);
    delete ui;
}

/******************************************************************************/
