TARGET = QtTestRunner
TEMPLATE = app
CONFIG += c++11
QT     += core gui widgets

SOURCES += \
    application.cpp \
    logging.cpp \
    main.cpp \
    maintask.cpp \
    mainwindow.cpp \
    unittestcollector.cpp \
    unittestrunner.cpp

HEADERS  += \
    application.h \
    logging.h \
    maintask.h \
    mainwindow.h \
    unittestcollector.h \
    unittestrunner.h \
    testsettings.h

FORMS += mainwindow.ui
