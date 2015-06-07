#ifndef TEST_SETTINGS_H
#define TEST_SETTINGS_H
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
        shuffle = true;
        repeat = 1;
        onebyone = true;
    }

    bool recursive;
    bool graphical;
    QString basepath;
    int nrjobs;
    bool debug;
    bool useldd;
    bool shuffle;
    int repeat;
    bool onebyone;
};

#endif // TEST_SETTINGS_H
