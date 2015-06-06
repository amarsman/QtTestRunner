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
    : m_semaphore(a_semaphore)
    , m_running(false)
    , m_stopRequested(false)
    , m_jobnr(0)
{
    qCDebug(LogQtTestRunnerCore);
}

/******************************************************************************/
UnitTestRunner::~UnitTestRunner()
{
    qCDebug(LogQtTestRunnerCore);
}

/******************************************************************************/
bool UnitTestRunner::start(int a_jobnr, const QString &a_unitTest)
{
    qCDebug(LogQtTestRunnerCore);
    if (m_running)
    {
        return false;
    }

    m_unitTest = a_unitTest;
    m_stopRequested = false;
    m_jobnr = a_jobnr;

    QThreadPool::globalInstance()->start(this);

    return true;
}

/******************************************************************************/
bool UnitTestRunner::stop()
{
    qCDebug(LogQtTestRunnerCore);
    if (!m_running) return false;

    qDebug("FileFinder Stopping...");
    m_stopRequested = true;

    return true;
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

    QFileInfo info(m_unitTest);
    QString testpath = info.absolutePath();

    QScopedPointer<UnitTestOutputHandler> handler(new UnitTestOutputHandler());
    handler->setUnitTestRunner(this);

    QScopedPointer<QProcess> process(new QProcess());
    process->start(m_unitTest, QStringList() << "-xml");
    process->waitForStarted();
    while (process->waitForReadyRead())
    {
        while (process->canReadLine())
        {
            QString line = QString(process->readLine());
            handler->processXmlLine(line.trimmed());
        }
    }

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
