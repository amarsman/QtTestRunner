#include <QScopedPointer>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QProcess>
#include <QThreadPool>
#include <QRegularExpression>
#include <QStringList>

#include "unittestcollector.h"
#include "unittestrunner.h"
#include "logging.h"

/******************************************************************************/
UnitTestCollector::UnitTestCollector()
    : m_stopRequested(false)
    , m_running(false)
{
    qCDebug(LogQtTestRunner);
    setAutoDelete(false);
}

/******************************************************************************/
UnitTestCollector::~UnitTestCollector()
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
void UnitTestCollector::start(const TestSettings &a_settings)
{
    qCDebug(LogQtTestRunner);
    if (m_running)
    {
        return;
    }
    m_settings = a_settings;
    sem.reset(new QSemaphore(m_settings.nrjobs));

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
void UnitTestCollector::onUnitTestResult(int jobnr, const QString &testResult, bool ok)
{
    emit unitTestResult(jobnr, testResult, ok);
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

    {
    QDirIterator it(m_settings.basepath,
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
            for (int i=0; i<m_settings.repeat; i++)
            {
                m_unitTests.append(fullPath);
                emit unittestFound(fullPath);
            }
        }
    }
    }

    qCDebug(LogQtTestRunner, "Sorting");
    if (m_settings.shuffle)
    {
        std::random_shuffle(m_unitTests.begin(), m_unitTests.end());
    }
    else
    {
        m_unitTests.sort();
    }


    int jobnr = 1;
    // Run all test executables
    for (auto it = m_unitTests.constBegin(); !m_stopRequested && it != m_unitTests.constEnd(); ++it)
    {
        QString filename = (*it);

        qCDebug(LogQtTestRunner, "Acquiring");
        sem->acquire();
        qCDebug(LogQtTestRunner, "Acquired");


        UnitTestRunner *runner = new UnitTestRunner(sem);

        QObject::connect(runner, &UnitTestRunner::unitTestResult,
                         this, &UnitTestCollector::onUnitTestResult);
        runner->start(jobnr, filename);
        jobnr++;
    }

    qCDebug(LogQtTestRunner, "Waiting for runners to finish");
    while (sem->available() != m_settings.nrjobs)
    {
        QThread::msleep(100);
    }

    m_running = false;
    qCDebug(LogQtTestRunner, "Finished");
    emit collectionFinished();
}

/******************************************************************************/
