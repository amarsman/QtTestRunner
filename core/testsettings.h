#ifndef TESTSETTINGS_H
#define TESTSETTINGS_H
#include <QString>

struct TestSettings
{
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
