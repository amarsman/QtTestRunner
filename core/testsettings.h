#ifndef TEST_SETTINGS_H
#define TEST_SETTINGS_H
#include <QString>

/** \brief Struct that holds all test options */
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

    QString basepath;    /*!< \brief Basepath where to look for tests */
    bool recursive;      /*!< \brief Look for tests recursively */
    int nrjobs;          /*!< \brief Run test in n threads */
    bool debug;          /*!< \brief Print debug info */
    bool useldd;         /*!< \brief Use ldd to determine if a file is a QTest */
    bool shuffle;        /*!< \brief Shuffle test order */
    int  repeat;         /*!< \brief Repeat tests n times */
    bool isolated;       /*!< \brief Run test functions one at a time */
    bool jhextensions;   /*!< \brief Use JH extensions (multi testcases) */
    int  verbosity;      /*!< \brief Set verbosity level at n */
    bool no_colors;      /*!< \brief Do not produce colored output */
};

#endif // TEST_SETTINGS_H
