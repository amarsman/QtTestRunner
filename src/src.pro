TARGET = QtTestRunner
TEMPLATE = app
CONFIG += c++11 link_prl
QT     += core gui widgets xml

DESTDIR = $${PWD}/../bin

SOURCES += logging.cpp \
           main.cpp \
           console_runner.cpp \
           unittestmodel.cpp

HEADERS  += logging.h \
            console_runner.h \
            unittestmodel.h

INCLUDEPATH += ../core
DEPENDPATH += ../core
LIBS += -L$$DESTDIR -lcore
TARGETDEPS += $$DESTDIR/libcore.a
