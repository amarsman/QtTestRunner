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
    unittestfinder.cpp

HEADERS  += \
    application.h \
    logging.h \
    maintask.h \
    mainwindow.h \
    unittestfinder.h

FORMS += mainwindow.ui
