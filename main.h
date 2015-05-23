#ifndef MAIN_H
#define MAIN_H
#include <QApplication>
#include <QLoggingCategory>

class TestRunnerApplication : public QApplication
{
public:
    TestRunnerApplication(int &argc, char **argv);
    virtual ~TestRunnerApplication();

    int run();

protected:
    void parseCommandLineOptions();

private:
    QString m_basepath;
    bool m_recursive;
    bool m_graphical;
    int m_nrjobs;

    QLoggingCategory category;
};


#endif // MAIN_H

