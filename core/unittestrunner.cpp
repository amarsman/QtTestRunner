#include <QThreadPool>
#include <QProcess>
#include <QRegularExpression>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QFileInfo>

#include "unittestrunner.h"
#include "logging.h"
#include "unittestoutputhandler.h"

/******************************************************************************/
UnitTestRunner::UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore)
    : m_semaphore(a_semaphore), m_running(false), m_stopRequested(false), m_jobnr(0)
{
    qCDebug(LogQtTestRunnerCore);

    re_tc_start         .setPattern("<TestCase name=\"(.*?)\">");
    re_tc_end           .setPattern("<\\/TestCase>");
    re_environment_start.setPattern("<Environment>");
    re_environment_end  .setPattern("<\\/Environment>");
    re_tf_start         .setPattern("<TestFunction name=\"(.*?)\">");
    re_tf_end           .setPattern("<\\/TestFunction>");
    re_incident         .setPattern("<Incident\\s+type=\"(.*?)\".*?\\/>");
    re_incident_start   .setPattern("<Incident\\s+type=\"(.*?)\".*?[^\\/]>");
    re_incident_end     .setPattern("<\\/Incident>");
    re_duration         .setPattern("<Duration.*?\\/>");

    m_inTestcase      = false;
    m_testCaseName     = "";
    m_inEnvironment   = false;
    m_inTestFunction  = false;
    m_testFunctionName = "";
    m_inIncident      = false;
}

/******************************************************************************/
UnitTestRunner::~UnitTestRunner()
{
    qCDebug(LogQtTestRunnerCore);
}

/******************************************************************************/
void UnitTestRunner::start(int a_jobnr, const QString &a_unitTest)
{
    qCDebug(LogQtTestRunnerCore);
    if (m_running)
    {
        return;
    }

    m_unitTest = a_unitTest;
    m_stopRequested = false;
    m_jobnr = a_jobnr;

    QThreadPool::globalInstance()->start(this);}

/******************************************************************************/
void UnitTestRunner::stop()
{
    qCDebug(LogQtTestRunnerCore);
    if (!m_running) return;

    qDebug("FileFinder Stopping...");
    m_stopRequested = true;
}

/******************************************************************************/
void UnitTestRunner::processXmlLine(const QString &line)
{
   //qCDebug(LogQtTestRunnerCore, "%s", line.toStdString().c_str());
   fprintf(stderr, "%s", line.toStdString().c_str());
}

/******************************************************************************/
void UnitTestRunner::run()
{
    qCDebug(LogQtTestRunnerCore, "Starting");

    if (m_running)
    {
        return;
    }

    m_running = true;

    qCDebug(LogQtTestRunnerCore, "%s ", m_unitTest.toStdString().c_str());

    QScopedPointer<QProcess> process(new QProcess());

    QFileInfo info(m_unitTest);
    QString testpath = info.absolutePath();


    QScopedPointer<QXmlSimpleReader> parser(new QXmlSimpleReader());
    QScopedPointer<UnitTestOutputHandler> handler(new UnitTestOutputHandler(this, testpath));

    parser->setContentHandler(handler.data());

    process->start(m_unitTest, QStringList() << "-xml");//, QStringList() << "-datatags");
#if 1
    parser->parse(new QXmlInputSource(process.data()));
#else
    process->waitForStarted();
    while (process->waitForReadyRead())
    {
        while (process->canReadLine())
        {
            QString line = QString(process->readLine());
            processXmlLine(line);
        }
    }
#endif

    process->waitForFinished();

    bool result_ok = true;
    QProcess::ExitStatus status = process->exitStatus();
    if (status  != QProcess::NormalExit)
    {
        result_ok = false;
    }
    else if (0 != process->exitCode())
    {
        result_ok = false;
    }

    m_running = false;
    qCDebug(LogQtTestRunnerCore, "Finished %s", result_ok ? "OK":"NOK");
    m_semaphore->release();
}

/******************************************************************************/
