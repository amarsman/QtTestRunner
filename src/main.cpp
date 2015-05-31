#include <QLoggingCategory>
#include <QDateTime>

#include <QCommandLineParser>
#include <QProcess>
#include <QTimer>
#include <thread>
#include <unistd.h>
#include <sys/types.h>

#include "application.h"
#include "gui_runner.h"
#include "console_runner.h"
#include "logging.h"
#include "testmanager.h"

/******************************************************************************/
void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch (type)
    {

    case QtDebugMsg:
        fprintf(stderr, "%s %s.debug %s.%u %s %s\n",
                QDateTime::currentDateTime().toString("HH:mm:ss.zzz").toStdString().c_str(),
                context.category,
                context.file,
                context.line,
                context.function,
                msg.toStdString().c_str());
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s %s.warng  %s.%u %s %s\n",
                QDateTime::currentDateTime().toString("HH:mm:ss.zzz").toStdString().c_str(),
                context.category,
                context.file,
                context.line,
                context.function,
                msg.toStdString().c_str());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s %s.error %s.%u %s %s\n",
                QDateTime::currentDateTime().toString("HH:mm:ss.zzz").toStdString().c_str(),
                context.category,
                context.file,
                context.line,
                context.function,
                msg.toStdString().c_str());
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s %s.fatal %s.%u %s %s\n",
                QDateTime::currentDateTime().toString("HH:mm:ss.zzz").toStdString().c_str(),
                context.category,
                context.file,
                context.line,
                context.function,
                msg.toStdString().c_str());
        break;
    }
}

/******************************************************************************/
static void parseCommandLineOptions(QCoreApplication &app,
                                    TestSettings &a_settings)
{
    /*
    Usage: src/QtTestRunner [options] path

    Options:
        -r, --recursive       Search recursive
        -g, --graphical       Use graphical interface
        -j, --jobs <nrjobs>   Use parallel jobs (default=nr_cpus)
        -d, --debug           Produce debug output
        -n, --repeat <count>  Repeat tests (default=1)
        -s, --shuffle         Shuffle tests
        -v, --version         Displays version information.
        -h, --help            Displays this help.

    Arguments:
        path                  Path or test executable. (default=cwd)
    */
    qCDebug(LogQtTestRunner);


    QCommandLineOption recursiveOption(QStringList() << "r" << "recursive", "Search recursive"                             );
    QCommandLineOption graphicalOption(QStringList() << "g" << "graphical", "Use graphical interface"                      );
    QCommandLineOption parallelOption (QStringList() << "j" << "jobs",      "Use parallel jobs (default=nr_cpus)", "nrjobs");
    QCommandLineOption debugOption    (QStringList() << "d" << "debug",     "Produce debug output"                         );
    QCommandLineOption repeatOption   (QStringList() << "n" << "repeat",    "Repeat tests (default=1)",            "count" );
    QCommandLineOption shuffleOption  (QStringList() << "s" << "shuffle",   "Shuffle tests"                                );

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
    parser.process(app);

    QStringList args = parser.positionalArguments();
    if (args.length() > 1) { parser.showHelp(-1); }

    a_settings.basepath = (args.length() < 1) ? "/home/henklaak/Projects/QtTestRunner" : args[0];
    a_settings.recursive = parser.isSet(recursiveOption);
    a_settings.graphical = parser.isSet(graphicalOption);
    a_settings.debug = parser.isSet(debugOption);
    a_settings.shuffle = parser.isSet(shuffleOption);

    bool valid=false;
    int nrjobs = parser.value(parallelOption).toInt(&valid);
    if (nrjobs < 1) valid = false;
    a_settings.nrjobs = valid ? nrjobs : 1; //std::thread::hardware_concurrency();

    valid=false;
    int nrrepeats = parser.value(repeatOption).toInt(&valid);
    if (nrrepeats < 1) valid = false;
    a_settings.repeat = valid ? nrrepeats : 1;

    if (a_settings.debug)
    {
        QLoggingCategory::setFilterRules("QtTestRunner.debug=true");
    }

    qCDebug(LogQtTestRunner, "basepath  %s", a_settings.basepath.toStdString().c_str());
    qCDebug(LogQtTestRunner, "recursive %s", a_settings.recursive ? "yes" : "no");
    qCDebug(LogQtTestRunner, "graphical %s", a_settings.graphical ? "yes" : "no");
    qCDebug(LogQtTestRunner, "nrjobs    %d", a_settings.nrjobs);
    qCDebug(LogQtTestRunner, "debug     %s", a_settings.debug ? "yes" : "no");
    qCDebug(LogQtTestRunner, "shuffle   %s", a_settings.shuffle ? "yes" : "no");
}

/******************************************************************************/
static void checkPreconditions(TestSettings &a_settings)
{
    qCDebug(LogQtTestRunner);

    // Find out if user is root
    {
        bool isroot = (0 == geteuid());
        if (!isroot)
        {
            fprintf(stdout, "Not root, only accessible tests will be run.\n");
        }
        qCDebug(LogQtTestRunner, "isroot: %d", isroot);
    }

    // Find out if ldd works
    {
        QScopedPointer<QProcess> process(new QProcess());

        process->start("ldd", QStringList() << "/bin/ls");
        a_settings.useldd = process->waitForFinished();
        qCDebug(LogQtTestRunner, "useldd: %d", a_settings.useldd);
    }
}

/******************************************************************************/
int main(int argc, char *argv[])
{
    qInstallMessageHandler(LogHandler);
    QLoggingCategory::setFilterRules("QtTestRunner.debug=false\n"
                                     "QtTestRunnerCore.debug=false");

    QApplication app(argc, argv);
    app.setOrganizationName("Heidenhain");
    app.setApplicationName("QtTestRunner"),
    app.setApplicationVersion("1.0");

    TestSettings test_settings;
    parseCommandLineOptions(app, test_settings);
    checkPreconditions(test_settings);

    TestManager test_manager;
    if (test_settings.graphical)
    {
        GuiRunner runner(&test_manager, &test_settings);
        runner.show();
        return app.exec();
    }
    else
    {
        ConsoleRunner runner(&test_manager, &test_settings);
        QObject::connect(&runner, &ConsoleRunner::finished,
                         &app, &TestRunnerApplication::quit);
        QTimer::singleShot(0, &runner, &ConsoleRunner::onRun);
        return app.exec();
    }
}

/******************************************************************************/
