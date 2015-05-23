#include <QApplication>
#include <QCommandLineParser>
#include <QTimer>
#include <thread>

#include "main.h"
#include "mainwindow.h"
#include "maintask.h"

/******************************************************************************/
TestRunnerApplication::TestRunnerApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_basepath(".")
    , m_recursive(false)
    , m_graphical(false)
    , m_nrjobs(4)
    , category("TestRunnerApplication")
{
    qCDebug(category);
    parseCommandLineOptions();
}

/******************************************************************************/
TestRunnerApplication::~TestRunnerApplication()
{
    qCDebug(category);
}

/******************************************************************************/
void TestRunnerApplication::parseCommandLineOptions()
{
    /*
    Usage: ./QtTestRunner [options] path

    Options:
        -r             Search recursive
        -g             Use graphical interface
        -j <nrjobs>    Use parallel jobs (default=nr_cpus)
        -v, --version  Displays version information.
        -h, --help     Displays this help.

    Arguments:
        path           Path or test executable. (default=cwd)
    */
    qCDebug(category);

    setApplicationName("QtTestRunner");
    setApplicationVersion("1.0");

    QCommandLineOption recursiveOption("r", "Search recursive");
    QCommandLineOption graphicalOption("g", "Use graphical interface");
    QCommandLineOption jobsOption("j", "Use parallel jobs (default=nr_cpus)", "nrjobs");

    QCommandLineParser parser;
    parser.addPositionalArgument("path", "Path or test executable. (default=cwd)");
    parser.addOption(recursiveOption);
    parser.addOption(graphicalOption);
    parser.addOption(jobsOption);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(*this);

    QStringList args = parser.positionalArguments();
    if (args.length() > 1) { parser.showHelp(-1); }

    m_basepath  = (args.length() < 1) ? "." : args[0];
    m_recursive = parser.isSet(recursiveOption);
    m_graphical = parser.isSet(graphicalOption);
    bool valid=false;
    int nrjobs = parser.value(jobsOption).toInt(&valid);
    if (nrjobs < 1) valid = false;
    m_nrjobs = valid ? nrjobs : std::thread::hardware_concurrency();
}

/******************************************************************************/
int TestRunnerApplication::run()
{
    qCDebug(category, "Parallel jobs: %d", m_nrjobs);

    if (m_graphical)
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
void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch (type)
    {
    case QtDebugMsg:
        fprintf(stderr, "DBG %s.%u %s %s\n", context.file, context.line, context.function, msg.toStdString().c_str());
        break;
    case QtWarningMsg:
        fprintf(stderr, "WAR %s.%u %s %s\n", context.file, context.line, context.function, msg.toStdString().c_str());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "ERR %s.%u %s %s\n", context.file, context.line, context.function, msg.toStdString().c_str());
        break;
    case QtFatalMsg:
        fprintf(stderr, "FTL %s.%u %s %s\n", context.file, context.line, context.function, msg.toStdString().c_str());
        break;
    }
}

/******************************************************************************/
int main(int argc, char *argv[])
{
    qInstallMessageHandler(LogHandler);
    TestRunnerApplication app(argc, argv);
    return app.run();
}

/******************************************************************************/
