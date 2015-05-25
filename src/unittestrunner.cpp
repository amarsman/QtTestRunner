#include <QThreadPool>
#include <QProcess>
#include <QRegularExpression>

#include "unittestrunner.h"
#include "logging.h"

/******************************************************************************/
UnitTestRunner::UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore)
    : m_semaphore(a_semaphore), m_running(false), m_stopRequested(false), m_jobnr(0)
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
UnitTestRunner::~UnitTestRunner()
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
void UnitTestRunner::start(int a_jobnr, const QString &a_unitTest)
{
    qCDebug(LogQtTestRunner);
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
    qCDebug(LogQtTestRunner);
    if (!m_running) return;

    qDebug("FileFinder Stopping...");
    m_stopRequested = true;
}

/******************************************************************************/
void UnitTestRunner::run()
{
    qCDebug(LogQtTestRunner, "Starting");

    if (m_running)
    {
        return;
    }

    m_running = true;

    qCDebug(LogQtTestRunner, "%s ", m_unitTest.toStdString().c_str());

    QScopedPointer<QProcess> process(new QProcess());

    process->start(m_unitTest);//, QStringList() << "-datatags");
    process->waitForStarted();
    while (process->waitForReadyRead())
    {
        while (process->canReadLine())
        {
            QString line = QString(process->readLine());
            if (line.startsWith("******")) continue;
            if (line.startsWith("Config:")) continue;
            if (line.startsWith("Totals:")) continue;
            //if (line.isEmpty()) continue;
            fprintf(stderr, "%s", line.toStdString().c_str());
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

    emit unitTestResult(m_jobnr, "Result:", result_ok);

    m_running = false;
    qCDebug(LogQtTestRunner, "Finished %s", result_ok ? "OK":"NOK");
    m_semaphore->release();
}

/******************************************************************************/
