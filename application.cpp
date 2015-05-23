#include <QCommandLineParser>
#include <QTimer>
#include <thread>
#include <unistd.h>
#include <sys/types.h>

#include "application.h"
#include "mainwindow.h"
#include "maintask.h"
#include "logging.h"
#include "unittestfinder.h"

/******************************************************************************/
TestRunnerApplication::TestRunnerApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_basepath(".")
    , m_recursive(false)
    , m_graphical(false)
    , m_nrjobs(4)
    , m_useldd(false)
    , m_isroot(false)
{
    qCDebug(LogQtTestRunner);
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

    m_basepath  = (args.length() < 1) ? "." : args[0];
    m_recursive = parser.isSet(recursiveOption);
    m_graphical = parser.isSet(graphicalOption);
    bool valid=false;
    int nrjobs = parser.value(parallelOption).toInt(&valid);
    if (nrjobs < 1) valid = false;
    m_nrjobs = valid ? nrjobs : std::thread::hardware_concurrency();
}

/******************************************************************************/
void TestRunnerApplication::checkPreconditions()
{
    qCDebug(LogQtTestRunner);

    // Find out if user is root
    {
        // Check dat ldd can be run
        m_isroot = (0 == geteuid());
        if (!m_isroot)
        {
            fprintf(stderr, "Not root, only accessible tests will be run.\n");
        }
        qCDebug(LogQtTestRunner, "isroot: %d", m_isroot);
    }

    // Find out if ldd works
    {
        FILE *pipe = popen("ldd /bin/ls", "r");
        if (pipe)
        {
            char buffer[256];
            while(!feof(pipe))
            {
                fgets(buffer, 256, pipe);
            }
            m_useldd = (0 == pclose(pipe)); //no errors
        }
        qCDebug(LogQtTestRunner, "useldd: %d", m_useldd);
    }
}


/******************************************************************************/
int TestRunnerApplication::run()
{
    parseCommandLineOptions();
    checkPreconditions();

    if (m_graphical)
    {
        MainWindow w;
        w.show();
        return exec();
    }
    else
    {
        UnitTestFinder finder;
        MainTask *task = new MainTask(this, m_basepath, &finder);
        QTimer::singleShot(0, task, SLOT(run()));
        QObject::connect(task, &MainTask::finished,
                         this, &TestRunnerApplication::quit);
        return exec();
    }
}

/******************************************************************************/
