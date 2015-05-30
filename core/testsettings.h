#ifndef TESTSETTINGS_H
#define TESTSETTINGS_H
#include <QString>

struct TestSettings
{
    TestSettings() {
        recursive = false;
        graphical = false;
        basepath = ".";
        nrjobs = 1;
        debug = false;
        useldd = true;
        shuffle = false;
        repeat = 1;
    }

    bool recursive;
    bool graphical;
    QString basepath;
    int nrjobs;
    bool debug;
    bool useldd;
    bool shuffle;
    int repeat;
};

#endif // TESTSETTINGS_H
