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
    QCommandLineOption repeatOption(QStringList() << "n" << "repeat", "Repeat tests", "count");
    QCommandLineOption shuffleOption(QStringList() << "s" << "shuffle", "Shuffle tests");

    QCommandLineParser parser;
    parser.addPositionalArgument("path", "Path or test executable. (default=cwd)");
    parser.addOption(recursiveOption);
    parser.addOption(graphicalOption);
    parser.addOption(parallelOption);
    parser.addOption(debugOption);
    parser.addOption(repeatOption);
    parser.addOption(shuffleOption);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(*this);

    QStringList args = parser.positionalArguments();
    if (args.length() > 1) { parser.showHelp(-1); }

    m_settings.basepath = (args.length() < 1) ? "/home/henklaak/Projects/QtCmake/build" : args[0];
    m_settings.recursive = parser.isSet(recursiveOption);
    m_settings.graphical = parser.isSet(graphicalOption);
    m_settings.debug = parser.isSet(debugOption);
    m_settings.shuffle = parser.isSet(shuffleOption);

    bool valid=false;
    int nrjobs = parser.value(parallelOption).toInt(&valid);
    if (nrjobs < 1) valid = false;
    m_settings.nrjobs = valid ? nrjobs : std::thread::hardware_concurrency();

    valid=false;
    int nrrepeats = parser.value(repeatOption).toInt(&valid);
    if (nrrepeats < 1) valid = false;
    m_settings.repeat = valid ? nrrepeats : 1;

    if (m_settings.debug)
    {
        QLoggingCategory::setFilterRules("QtTestRunner.debug=true");
    }

    qCDebug(LogQtTestRunner, "basepath  %s", m_settings.basepath.toStdString().c_str());
    qCDebug(LogQtTestRunner, "recursive %s", m_settings.recursive ? "yes" : "no");
    qCDebug(LogQtTestRunner, "graphical %s", m_settings.graphical ? "yes" : "no");
    qCDebug(LogQtTestRunner, "nrjobs    %d", m_settings.nrjobs);
    qCDebug(LogQtTestRunner, "debug     %s", m_settings.debug ? "yes" : "no");
    qCDebug(LogQtTestRunner, "shuffle   %s", m_settings.shuffle ? "yes" : "no");
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
        m_settings.useldd = process->waitForFinished();
        qCDebug(LogQtTestRunner, "useldd: %d", m_settings.useldd);
    }
}


/******************************************************************************/
int TestRunnerApplication::run()
{
    parseCommandLineOptions();
    checkPreconditions();

    if (m_settings.graphical)
    {
        MainWindow w;
        w.show();
        return exec();
    }
    else
    {
        MainTask *task = new MainTask(this, m_settings);
        QTimer::singleShot(0, task, SLOT(run()));
        QObject::connect(task, &MainTask::finished,
                         this, &TestRunnerApplication::quit);
        return exec();
    }
}

/******************************************************************************/
