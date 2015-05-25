#include <QScopedPointer>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QProcess>
#include <QThreadPool>
#include <QRegularExpression>
#include <QStringList>
#include <QSettings>

#include "unittestcollector.h"
#include "unittestrunner.h"
#include "logging.h"

/******************************************************************************/
UnitTestCollector::UnitTestCollector()
    : m_searchPath("")
    , m_stopRequested(false)
    , m_running(false)
{
    qCDebug(LogQtTestRunner);
    setAutoDelete(false);
    QSettings settings;
    m_nrjobs = settings.value("nrjobs", 1).toInt();
    sem.reset(new QSemaphore(m_nrjobs));
}

/******************************************************************************/
UnitTestCollector::~UnitTestCollector()
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
void UnitTestCollector::start(const QString &a_SearchPath)
{
    qCDebug(LogQtTestRunner);
    if (m_running)
    {
        return;
    }

    m_searchPath = a_SearchPath;
    m_stopRequested = false;

    QThreadPool::globalInstance()->start(this);
}

/******************************************************************************/
void UnitTestCollector::stop()
{
    qCDebug(LogQtTestRunner);
    if (!m_running) return;

    qDebug("FileFinder Stopping...");
    m_stopRequested = true;
}

/******************************************************************************/
bool UnitTestCollector::isUnitTest(const QString &filename)
{
    Q_UNUSED(filename);

    QScopedPointer<QProcess> process(new QProcess());

    process->start("ldd",QStringList() << filename);
    process->waitForFinished();
    QString data = QString(process->readAllStandardOutput());


    QRegularExpression re("libQt5Test");

    return (re.match(data).hasMatch());
}

/******************************************************************************/
void UnitTestCollector::run()
{
    qCDebug(LogQtTestRunner, "Starting");

    if (m_running)
    {
        qCDebug(LogQtTestRunner, "Finished");
        emit collectionFinished();
        return;
    }

    m_running = true;

    // Collect all tests executables
    QStringList m_unitTests;

    QDirIterator it(m_searchPath,
                    QStringList() << "*",
                    QDir::Files | QDir::Executable,
                    QDirIterator::Subdirectories);

    while (!m_stopRequested && it.hasNext())
    {
        it.next();
        QDir dir(it.filePath());

        qCDebug(LogQtTestRunner, "%s", dir.absolutePath().toStdString().c_str());

        QString fullPath = dir.absolutePath();

        if (isUnitTest(fullPath))
        {
            m_unitTests.append(fullPath);
            emit unittestFound(fullPath);
        }
    }

    qCDebug(LogQtTestRunner, "Sorting");
    m_unitTests.sort();

    // Run all test executables
    for (auto it = m_unitTests.constBegin(); !m_stopRequested && it != m_unitTests.constEnd(); ++it)
    {
        QString filename = (*it);

        qCDebug(LogQtTestRunner, "Acquiring");
        sem->acquire();
        qCDebug(LogQtTestRunner, "Acquired");


        UnitTestRunner *runner = new UnitTestRunner(sem);
        runner->start(filename);
    }

    qCDebug(LogQtTestRunner, "Waiting for runners to finish");
    while (sem->available() != m_nrjobs)
    {
        QThread::msleep(100);
    }

    m_running = false;
    qCDebug(LogQtTestRunner, "Finished");
    emit collectionFinished();
}

/******************************************************************************/
