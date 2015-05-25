#include <QCommandLineParser>
#include <QProcess>
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
    , m_basepath("")
    , m_recursive(false)
    , m_graphical(false)
    , m_nrjobs(1)
    , m_debug(false)
    , m_useldd(true)
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
    QCommandLineOption debugOption(QStringList() << "d" << "debug", "Produce debug output");

    QCommandLineParser parser;
    parser.addPositionalArgument("path", "Path or test executable. (default=cwd)");
    parser.addOption(recursiveOption);
    parser.addOption(graphicalOption);
    parser.addOption(parallelOption);
    parser.addOption(debugOption);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(*this);

    QStringList args = parser.positionalArguments();
    if (args.length() > 1) { parser.showHelp(-1); }

    m_basepath = (args.length() < 1) ? "/home/henklaak/Projects/QtCmake/build" : args[0];
    m_recursive = parser.isSet(recursiveOption);
    m_graphical = parser.isSet(graphicalOption);
    bool valid=false;
    int nrjobs = parser.value(parallelOption).toInt(&valid);
    if (nrjobs < 1) valid = false;
    m_nrjobs = valid ? nrjobs : std::thread::hardware_concurrency();
    m_debug = parser.isSet(debugOption);

    if (m_debug)
    {
        QLoggingCategory::setFilterRules("QtTestRunner.debug=true");
    }

    qCDebug(LogQtTestRunner, "basepath  %s", m_basepath.toStdString().c_str());
    qCDebug(LogQtTestRunner, "recursive %s", m_recursive ? "yes" : "no");
    qCDebug(LogQtTestRunner, "graphical %s", m_graphical ? "yes" : "no");
    qCDebug(LogQtTestRunner, "nrjobs    %d", m_nrjobs);
    qCDebug(LogQtTestRunner, "debug     %s", m_debug ? "yes" : "no");
}

/******************************************************************************/
void TestRunnerApplication::checkPreconditions()
{
    qCDebug(LogQtTestRunner);

    // Find out if user is root
    {
        bool isroot = (0 == geteuid());
        if (!isroot)
        {
            fprintf(stderr, "Not root, only accessible tests will be run.\n");
        }
        qCDebug(LogQtTestRunner, "isroot: %d", isroot);
    }

    // Find out if ldd works
    {
        QScopedPointer<QProcess> process(new QProcess());

        process->start("ldd", QStringList() << "/bin/ls");
        m_useldd = process->waitForFinished();
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
        MainTask *task = new MainTask(this, m_basepath, m_nrjobs);
        QTimer::singleShot(0, task, SLOT(run()));
        QObject::connect(task, &MainTask::finished,
                         this, &TestRunnerApplication::quit);
        return exec();
    }
}

/******************************************************************************/
