#include <QLoggingCategory>
#include <QDateTime>

#include "application.h"

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
int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules("QtTestRunner.debug=false");
    qInstallMessageHandler(LogHandler);

    TestRunnerApplication app(argc, argv);
    return app.run();
}

/******************************************************************************/
