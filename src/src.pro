TARGET = QtTestRunner
TEMPLATE = app
CONFIG += c++11 link_prl
QT     += core gui widgets xml
DESTDIR = $${PWD}/../bin

SOURCES += \
    logging.cpp \
    main.cpp \
    console_runner.cpp \
    gui_runner.cpp \
    unittestmodel.cpp

HEADERS  += \
    logging.h \
    gui_runner.h \
    console_runner.h \
    unittestmodel.h \
    testsettings.h

FORMS += mainwindow.ui

DEPENDPATH += $$PWD/../core
INCLUDEPATH += $$PWD/../core
LIBS += -L$$OUT_PWD/../core/ -lcore

TARGETDEPS += $$OUT_PWD/../core/libcore.a
