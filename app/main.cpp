/*
Copyright (C) 2015 Henk van der Laak

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <unistd.h>

#include <QCommandLineParser>
#include <QApplication>
#include <QDateTime>
#include <QLoggingCategory>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlDebuggingEnabler>
#include <QThread>
#include <QTimer>

#include "console_runner.h"
#include "testmanager.h"
#include "testsettings.h"

Q_LOGGING_CATEGORY(LogQtTestRunner, "QtTestRunner")

static const char SEARCHPATH[] = ".";

QQmlDebuggingEnabler enabler;

/******************************************************************************/
void LogHandler(QtMsgType type,
                const QMessageLogContext &context,
                const QString &msg)
{
    switch (type)
    {

    case QtDebugMsg:
        fprintf(stdout, "%s %s.debug %s.%u %s %s\n",
                QDateTime::currentDateTime().toString("HH:mm:ss.zzz").toLatin1().data(),
                context.category,
                context.file,
                context.line,
                context.function,
                msg.toStdString().c_str());
        break;
    case QtWarningMsg:
        fprintf(stdout, "%s %s.warng  %s.%u %s %s\n",
                QDateTime::currentDateTime().toString("HH:mm:ss.zzz").toLatin1().data(),
                context.category,
                context.file,
                context.line,
                context.function,
                msg.toStdString().c_str());
        break;
    case QtCriticalMsg:
        fprintf(stdout, "%s %s.error %s.%u %s %s\n",
                QDateTime::currentDateTime().toString("HH:mm:ss.zzz").toLatin1().data(),
                context.category,
                context.file,
                context.line,
                context.function,
                msg.toStdString().c_str());
        break;
    case QtFatalMsg:
        fprintf(stdout, "%s %s.fatal %s.%u %s %s\n",
                QDateTime::currentDateTime().toString("HH:mm:ss.zzz").toLatin1().data(),
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
    Usage: ./QtTestRunner [options] path

    Options:
        -r, --recursive       Search recursive
        -j, --jobs <nrjobs>   Use parallel jobs (default=nr_cpus)
        -d, --debug           Produce debug output
        -n, --repeat <count>  Repeat tests (default=1)
        -s, --shuffle         Shuffle tests
        -i, --individual      Run tests individually
        -v, --version         Displays version information.
        -h, --help            Displays this help.

    Arguments:
        path                  Path or test executable. (default=cwd)
    */
    qCDebug(LogQtTestRunner);

    QCommandLineOption recursiveOption(QStringList() << "r" << "recursive",  "Search recursive"                             );
    QCommandLineOption parallelOption (QStringList() << "j" << "jobs",       "Use parallel jobs (default=nr_cpus)", "nrjobs");
    QCommandLineOption debugOption    (QStringList() << "d" << "debug",      "Produce debug output"                         );
    QCommandLineOption repeatOption   (QStringList() << "n" << "repeat",     "Repeat tests (default=1)",            "count" );
    QCommandLineOption shuffleOption  (QStringList() << "s" << "shuffle",    "Shuffle tests"                                );
    QCommandLineOption singleOption   (QStringList() << "i" << "individual", "Run tests individually"                       );
    QCommandLineOption jhOption       (QStringList() << "jh",         "Disable JH extensions"                        );
    QCommandLineOption nocolorOption  (QStringList() << "c" << "no-color",   "Disabled color output"                        );
    QCommandLineOption verboseOption  (QStringList() << "V" << "verbosity",  "Set verbosity (default=1)",            "verb" );
    QCommandLineOption guiOption      (QStringList() << "g" << "gui",        "Use GUI"                                      );

    QCommandLineParser parser;
    parser.addPositionalArgument("path", "Path or test executable. (default=cwd)");
    parser.addOption(recursiveOption);
    parser.addOption(parallelOption);
    parser.addOption(debugOption);
    parser.addOption(repeatOption);
    parser.addOption(shuffleOption);
    parser.addOption(singleOption);
    parser.addOption(jhOption);
    parser.addOption(verboseOption);
    parser.addOption(nocolorOption);
    parser.addOption(guiOption);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    QStringList args = parser.positionalArguments();
    if (args.length() > 1) {
        parser.showHelp(-1);
    }

    a_settings.basepath = (args.length() < 1) ? SEARCHPATH : args[0];
    a_settings.recursive = parser.isSet(recursiveOption);
    a_settings.debug = parser.isSet(debugOption);
    a_settings.shuffle = parser.isSet(shuffleOption);
    a_settings.isolated = parser.isSet(singleOption);
    a_settings.jhextensions = !parser.isSet(jhOption);
    a_settings.no_colors = parser.isSet(nocolorOption);
    a_settings.graphical = parser.isSet(guiOption);

    bool valid=false;
    int nrjobs = parser.value(parallelOption).toInt(&valid);
    if (nrjobs < 1) valid = false;
    int nrthreads = QThread::idealThreadCount();
    a_settings.nrjobs = valid ? nrjobs : ((nrthreads > 0) ? nrthreads : 1);

    valid=false;
    int nrrepeats = parser.value(repeatOption).toInt(&valid);
    if (nrrepeats < 1) valid = false;
    a_settings.repeat = valid ? nrrepeats : 1;

    valid=false;
    int verbosity = parser.value(verboseOption).toInt(&valid);
    if (verbosity < 0) valid = false;
    a_settings.verbosity = valid ? verbosity : 1;
    if (a_settings.debug)
    {
        QLoggingCategory::setFilterRules("QtTestRunner.debug=true\n"
                                         "QtTestRunnerCoreHandler.debug=false");
    }

    // Debug settings
    //a_settings.shuffle = true;
    a_settings.isolated = true;
    //a_settings.repeat = 100;
    a_settings.nrjobs = 1;
    //a_settings.jhextensions = false;
    //a_settings.verbosity = 2;

    qCDebug(LogQtTestRunner, "basepath  %s", a_settings.basepath.toLatin1().data());
    qCDebug(LogQtTestRunner, "recursive %s", a_settings.recursive ? "yes" : "no");
    qCDebug(LogQtTestRunner, "nrjobs    %d", a_settings.nrjobs);
    qCDebug(LogQtTestRunner, "debug     %s", a_settings.debug ? "yes" : "no");
    qCDebug(LogQtTestRunner, "useldd    %s", a_settings.useldd ? "yes" : "no");
    qCDebug(LogQtTestRunner, "shuffle   %s", a_settings.shuffle ? "yes" : "no");
    qCDebug(LogQtTestRunner, "repeat    %d", a_settings.repeat);
    qCDebug(LogQtTestRunner, "isolated  %s", a_settings.isolated ? "yes" : "no");
    qCDebug(LogQtTestRunner, "jhext     %s", a_settings.jhextensions ? "yes" : "no");
    qCDebug(LogQtTestRunner, "verbosity %d", a_settings.verbosity);
    qCDebug(LogQtTestRunner, "no colors %s", a_settings.no_colors ? "yes" : "no");
    qCDebug(LogQtTestRunner, "gui       %s", a_settings.graphical ? "yes" : "no");
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
        a_settings.useldd = false;

        QScopedPointer<QProcess> process(new QProcess());

        process->start("ldd", QStringList() << "/bin/ls");
        if (process->waitForFinished(5000) &&
                process->exitStatus() == QProcess::NormalExit &&
                process->exitCode() == 0)
        {
            a_settings.useldd = true;
        }
        qCDebug(LogQtTestRunner, "useldd: %d", a_settings.useldd);
    }
}

/******************************************************************************/
int main(int argc, char *argv[])
{
    qInstallMessageHandler(LogHandler);
    QLoggingCategory::setFilterRules("QtTestRunner.debug=false\n"
                                     "QtTestRunnerCore.debug=false\n"
                                     "QtTestRunnerCoreHandler.debug=false");

    QApplication app(argc, argv);
    app.setOrganizationName("Heidenhain");
    app.setApplicationName("QtTestRunner");
    app.setApplicationVersion("1.0");

    TestManager test_manager;

    TestSettings test_settings;
    parseCommandLineOptions(app, test_settings);
    checkPreconditions(test_settings);

    ConsoleRunner runner(&test_manager, &test_settings);

    if (!test_settings.graphical)
    {
        QObject::connect(&runner, &ConsoleRunner::testingFinished,
                         &app, &QApplication::exit);
        QTimer::singleShot(0, &runner, &ConsoleRunner::onRun);
        return app.exec();
    }
    else
    {
        //QObject::connect(&runner, &ConsoleRunner::testingFinished,
        //                 &app, &QApplication::exit);
        //QTimer::singleShot(0, &runner, &ConsoleRunner::onRun);

        QQmlApplicationEngine engine;
        engine.rootContext()->setContextProperty("ConsoleRunner", &runner);
        engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));

        return app.exec();
    }
}

/******************************************************************************/
