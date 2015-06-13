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
bool UnitTestRunner::start(int a_jobnr,
                           const QString &a_testSuiteName,
                           const QString &a_testCaseName,
                           const QString &a_testFunctionName)
{
    qCDebug(LogQtTestRunnerCore);
    if (m_running)
    {
        return false;
    }

    m_testSuiteName = a_testSuiteName;
    m_testCaseName = a_testCaseName;
    m_testFunctionName = a_testFunctionName;
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
        m_semaphore->release();
        return;
    }

    m_running = true;

    qCDebug(LogQtTestRunnerCore, "%s ", m_testSuiteName.toLatin1().data());

    QFileInfo info(m_testSuiteName);
    QString testpath = info.absoluteFilePath();

    QScopedPointer<UnitTestOutputHandler> handler(new UnitTestOutputHandler(testpath));
    handler->setUnitTestRunner(this);

    QScopedPointer<QProcess> process(new QProcess());
    if (m_testFunctionName.isEmpty())
    {
        process->start(m_testSuiteName, QStringList() << "-o" << "-,xml");
    }
    else if (m_testCaseName.isEmpty())
    {
        process->start(m_testSuiteName, QStringList() << "-o" << "-,xml" << m_testFunctionName);
    }
    else
    {
        process->start(m_testSuiteName, QStringList() << "-o" << "-,xml" << "-testcase" << m_testCaseName << m_testFunctionName);
    }
    process->waitForStarted();
    while (process->waitForReadyRead())
    {
        while (process->canReadLine())
        {
            QString line = QString(process->readLine());
            handler->processXmlLine(line.trimmed());
        }
    }

    if (!process->waitForFinished(180000)) // 3 minutes
    {
        process->kill();
    }

    bool result_ok = true;
    QProcess::ExitStatus status = process->exitStatus();
    if (status  != QProcess::NormalExit)
    {
        emit crashTestSuite(handler->m_testSuite);
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
