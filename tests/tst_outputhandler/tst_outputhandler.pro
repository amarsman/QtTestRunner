TARGET = tst_outputhandler

include(../test_exec.pri)

SOURCES += \
    tst_outputhandler.cpp

TESTDATA = \
    testdata.txt

for(FILE,TESTDATA){
    QMAKE_POST_LINK += $$quote(cp $${PWD}/$${FILE} $${DESTDIR}$$escape_expand(\n\t))
}
