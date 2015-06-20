TARGET = QtTestRunner
TEMPLATE = app
CONFIG += c++11
QT     += quick core widgets xml

DESTDIR = $${PWD}/../bin

SOURCES += main.cpp \
           console_runner.cpp

HEADERS  += console_runner.h

INCLUDEPATH += ../core
DEPENDPATH += ../core
LIBS += -L$$DESTDIR -lcore
TARGETDEPS += $$DESTDIR/libcore.a

RESOURCES +=  bla.qrc
