#include <QThreadPool>
#include <QProcess>
#include <QRegularExpression>

#include "unittestrunner.h"
#include "logging.h"

/******************************************************************************/
UnitTestRunner::UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore)
 : m_semaphore(a_semaphore), m_running(false), m_stopRequested(false)
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
UnitTestRunner::~UnitTestRunner()
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
void UnitTestRunner::start(const QString &a_unitTest)
{
    qCDebug(LogQtTestRunner);
    if (m_running)
    {
        return;
    }

    m_unitTest = a_unitTest;
    m_stopRequested = false;

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
        process->waitForFinished();

        QString data = QString(process->readAllStandardOutput());
        QStringList lines = data.split(QRegularExpression("\n|\r\n|\r"));
        for (auto it2 = lines.constBegin(); it2 != lines.constEnd(); ++it2)
        {
            const QString &line = *it2;
            if (line.startsWith("******")) continue;
            if (line.startsWith("Config")) continue;
            if (line.startsWith("Totals")) continue;
            if (line.isEmpty()) continue;

            fprintf(stderr, "%s\n", line.toStdString().c_str());
        }

    m_running = false;
    qCDebug(LogQtTestRunner, "Finished");
    m_semaphore->release();
    emit testingFinished();
}

/******************************************************************************/
