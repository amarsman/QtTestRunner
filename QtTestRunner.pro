#-------------------------------------------------
#
# Project created by QtCreator 2015-05-23T10:40:56
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtTestRunner
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    maintask.cpp

HEADERS  += mainwindow.h \
    main.h \
    maintask.h

FORMS    += mainwindow.ui
