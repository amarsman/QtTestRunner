TEMPLATE = app
CONFIG += c++11 testcase
QT += testlib
DESTDIR = $${PWD}/../bin

INCLUDEPATH += ../../core
LIBS += -L$${DESTDIR} -lcore

