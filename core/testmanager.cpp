#include <QScopedPointer>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QProcess>
#include <QThreadPool>
#include <QRegularExpression>
#include <QStringList>

#include "testmanager.h"
#include "unittestrunner.h"
#include "logging.h"

/******************************************************************************/
TestManager::TestManager()
    : m_stopRequested(false)
    , m_running(false)
{
    qCDebug(LogQtTestRunnerCore);
    setAutoDelete(false);
}

/******************************************************************************/
TestManager::~TestManager()
{
    qCDebug(LogQtTestRunnerCore);
}

/******************************************************************************/
void TestManager::start(TestSettings *a_settings)
{
    qCDebug(LogQtTestRunnerCore);
    if (m_running)
    {
        return;
    }
    m_settings = a_settings;
    sem.reset(new QSemaphore(m_settings->nrjobs));

    QThreadPool::globalInstance()->start(this);
}

/******************************************************************************/
void TestManager::stop()
{
    qCDebug(LogQtTestRunnerCore);
    if (!m_running) return;

    qDebug("FileFinder Stopping...");
    m_stopRequested = true;
}

/******************************************************************************/
bool TestManager::isUnitTest(const QString &filename)
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
void TestManager::onTestCaseChanged(const TestCase &result)
{
    emit unitTestResult(result);
}

/******************************************************************************/
void TestManager::onEndTestFunction(const TestFunction &result)
{
    emit endTestFunction(result);
}

/******************************************************************************/
void TestManager::onEndTestCase(const TestCase &result)
{
    emit endTestCase(result);
}

/******************************************************************************/
void TestManager::run()
{
    qCDebug(LogQtTestRunnerCore, "Starting");

    if (m_running)
    {
        qCDebug(LogQtTestRunnerCore, "Finished");
        emit finished();
        return;
    }

    m_running = true;

    // Collect all tests executables
    QStringList m_unitTests;

    {
    QDirIterator it(m_settings->basepath,
                    QStringList() << "*",
                    QDir::Files | QDir::Executable,
                    QDirIterator::Subdirectories);

    while (!m_stopRequested && it.hasNext())
    {
        it.next();
        QDir dir(it.filePath());

        qCDebug(LogQtTestRunnerCore, "%s", dir.absolutePath().toStdString().c_str());

        QString fullPath = dir.absolutePath();

        if (isUnitTest(fullPath))
        {
            for (int i=0; i<m_settings->repeat; i++)
            {
                m_unitTests.append(fullPath);
                emit unitTestFound(fullPath);
            }
        }
    }
    }

    qCDebug(LogQtTestRunnerCore, "Sorting");
    if (m_settings->shuffle)
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

        qCDebug(LogQtTestRunnerCore, "Acquiring");
        sem->acquire();
        qCDebug(LogQtTestRunnerCore, "Acquired");


        UnitTestRunner *runner = new UnitTestRunner(sem);

        QObject::connect(runner, &UnitTestRunner::testCaseChanged,
                         this, &TestManager::onTestCaseChanged);
        QObject::connect(runner, &UnitTestRunner::endTestCase,
                         this, &TestManager::onEndTestCase);
        QObject::connect(runner, &UnitTestRunner::endTestFunction,
                         this, &TestManager::onEndTestFunction);

        runner->start(jobnr, filename);
        jobnr++;
    }

    qCDebug(LogQtTestRunnerCore, "Waiting for runners to finish");
    while (sem->available() != m_settings->nrjobs)
    {
        QThread::msleep(100);
    }

    m_running = false;
    qCDebug(LogQtTestRunnerCore, "Finished");
    emit finished();
}

/******************************************************************************/
