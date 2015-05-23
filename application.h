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

private:
    QString m_basepath;
    bool m_recursive;
    bool m_graphical;
    int m_nrjobs;
    bool m_useldd;
    bool m_isroot;
};


#endif // APPLICATION_H
