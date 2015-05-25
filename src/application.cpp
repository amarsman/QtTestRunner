#include <QCommandLineParser>
#include <QSettings>
#include <QTimer>
#include <thread>
#include <unistd.h>
#include <sys/types.h>

#include "application.h"
#include "mainwindow.h"
#include "maintask.h"
#include "logging.h"
#include "unittestcollector.h"

/******************************************************************************/
TestRunnerApplication::TestRunnerApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
    qCDebug(LogQtTestRunner);
    setOrganizationName("LSI");
    setApplicationName("QtTestRunner"),
    setApplicationVersion("1.0");
}

/******************************************************************************/
TestRunnerApplication::~TestRunnerApplication()
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
void TestRunnerApplication::parseCommandLineOptions()
{
    /*
    Usage: ./QtTestRunner [options] path

    Options:
        -r, --recursive      Search recursive
        -g, --graphical      Use graphical interface
        -j, --jobs <nrjobs>  Use parallel jobs (default=nr_cpus)
        -v, --version        Displays version information.
        -h, --help           Displays this help.

    Arguments:
        path                 Path or test executable. (default=cwd)
    */
    qCDebug(LogQtTestRunner);

    setApplicationName("QtTestRunner");
    setApplicationVersion("1.0");

    QCommandLineOption recursiveOption(QStringList() << "r" << "recursive", "Search recursive");
    QCommandLineOption graphicalOption(QStringList() << "g" << "graphical", "Use graphical interface");
    QCommandLineOption parallelOption(QStringList() << "j" << "jobs", "Use parallel jobs (default=nr_cpus)", "nrjobs");

    QCommandLineParser parser;
    parser.addPositionalArgument("path", "Path or test executable. (default=cwd)");
    parser.addOption(recursiveOption);
    parser.addOption(graphicalOption);
    parser.addOption(parallelOption);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(*this);

    QStringList args = parser.positionalArguments();
    if (args.length() > 1) { parser.showHelp(-1); }

    QSettings settings;
    settings.setValue("basepath", (args.length() < 1) ? "." : args[0]);
    settings.setValue("recursive", parser.isSet(recursiveOption));
    settings.setValue("graphical", parser.isSet(graphicalOption));
    bool valid=false;
    int nrjobs = parser.value(parallelOption).toInt(&valid);
    if (nrjobs < 1) valid = false;
    settings.setValue("nrjobs", valid ? nrjobs : std::thread::hardware_concurrency());

    //for testing
    settings.setValue("basepath", "/home/henklaak/Projects/QtCmake/build");
    settings.setValue("recursive", true);
    settings.setValue("graphical", false);
    settings.setValue("nrjobs", 8);
}

/******************************************************************************/
void TestRunnerApplication::checkPreconditions()
{
    qCDebug(LogQtTestRunner);

    // Find out if user is root
    {
        // Check dat ldd can be run
        bool isroot = (0 == geteuid());
        if (!isroot)
        {
            fprintf(stderr, "Not root, only accessible tests will be run.\n");
        }
        qCDebug(LogQtTestRunner, "isroot: %d", isroot);
    }

    // Find out if ldd works
    {
        bool useldd=false;
        FILE *pipe = popen("ldd /bin/ls", "r");
        if (pipe)
        {
            char buffer[256];
            while(!feof(pipe))
            {
                fgets(buffer, 256, pipe);
            }
            useldd = (0 == pclose(pipe)); // no errors
        }

        QSettings settings;
        settings.setValue("useldd", useldd);
        qCDebug(LogQtTestRunner, "useldd: %d", useldd);
    }
}


/******************************************************************************/
int TestRunnerApplication::run()
{
    parseCommandLineOptions();
    checkPreconditions();

    QSettings settings;

    if (settings.value("graphical", false).toBool())
    {
        MainWindow w;
        w.show();
        return exec();
    }
    else
    {
        MainTask *task = new MainTask(this);
        QTimer::singleShot(0, task, SLOT(run()));
        QObject::connect(task, &MainTask::finished,
                         this, &TestRunnerApplication::quit);
        return exec();
    }
}

/******************************************************************************/
