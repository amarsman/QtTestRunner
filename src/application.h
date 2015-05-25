#ifndef APPLICATION_H
#define APPLICATION_H
#include <QApplication>

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
};


#endif // APPLICATION_H

