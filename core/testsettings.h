#ifndef TEST_SETTINGS_H
#define TEST_SETTINGS_H
#include <QString>

struct TestSettings
{
    TestSettings() {
        basepath = ".";
        recursive = false;
        nrjobs = 1;
        debug = false;
        useldd = true;
        shuffle = true;
        repeat = 1;
        isolated = true;
        jhextensions = true;
        verbosity = 1;
        no_colors = false;
    }

    QString basepath;
    bool recursive;
    int nrjobs;
    bool debug;
    bool useldd;
    bool shuffle;
    int  repeat;
    bool isolated;
    bool jhextensions;
    int  verbosity;
    bool no_colors;
};

#endif // TEST_SETTINGS_H
