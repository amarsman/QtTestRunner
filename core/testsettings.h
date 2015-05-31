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

#endif // TEST_SETTINGS_H
