TEMPLATE = lib
TARGET = core
CONFIG += c++11 staticlib create_prl
QT += xml

SOURCES += \
    unittestrunner.cpp \
    testmanager.cpp \
    unittestoutputhandler.cpp

HEADERS += \
    unittestrunner.h \
    testsettings.h \
    testmanager.h \
    unittestoutputhandler.h

DESTDIR = $${PWD}/../bin
