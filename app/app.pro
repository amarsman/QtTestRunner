TARGET = QtTestRunner
TEMPLATE = app
CONFIG += c++11 link_prl
QT     += core gui widgets xml

DESTDIR = $${PWD}/../bin

SOURCES += main.cpp \
           console_runner.cpp

HEADERS  += console_runner.h

INCLUDEPATH += ../core
DEPENDPATH += ../core
LIBS += -L$$DESTDIR -lcore
TARGETDEPS += $$DESTDIR/libcore.a
