#ifndef APPLICATION_H
#define APPLICATION_H
#include <QApplication>
#include "testsettings.h"

class TestRunnerApplication : public QApplication
{
    Q_OBJECT
public:
    TestRunnerApplication(int &argc, char **argv);
    virtual ~TestRunnerApplication();

    int run();

protected:
    void parseCommandLineOptions();
    void checkPreconditions();

    TestSettings m_settings;
};


#endif // APPLICATION_H

