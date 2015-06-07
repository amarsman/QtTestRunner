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
    QScopedPointer<QProcess> process(new QProcess());

    process->start("ldd",QStringList() << filename);
    process->waitForFinished();
    QString data = QString(process->readAllStandardOutput());

    QRegularExpression re("libQt5Test");

    return (re.match(data).hasMatch());
}

/******************************************************************************/
QStringList TestManager::getTests(const QString &filename)
{
    QStringList tests;

    QScopedPointer<QProcess> process(new QProcess());

    process->start(filename, QStringList() << "-functions");
    process->waitForFinished();

    QStringList data = QString(process->readAllStandardOutput()).split('\n');

    for (auto it = data.begin(); it != data.end(); ++it)
    {
        QString &line = *it;

        if (line.endsWith("()"))
        {
            QString testname = line.left(line.length()-2);
            tests.append(testname);
        }
    }
    return tests;
}

/******************************************************************************/
void TestManager::onTestSuiteChanged(const TestSuite &a_testSuite)
{
    emit testSuiteChanged(a_testSuite);
}

/******************************************************************************/
void TestManager::onEndTestSuite(const TestSuite &a_testSuite)
{
    emit endTestSuite(a_testSuite);
}

/******************************************************************************/
void TestManager::onEndTestCase(const TestCase &a_testCase)
{
    emit endTestCase(a_testCase);
}

/******************************************************************************/
void TestManager::onEndTestFunction(const TestFunction &a_testFunction)
{
    emit endTestFunction(a_testFunction);
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
    QList<QPair<QString,QString>> m_unitTests;

    QDirIterator it(m_settings->basepath,
                    QStringList() << "*",
                    QDir::Files | QDir::Executable,
                    QDirIterator::Subdirectories);

    while (!m_stopRequested && it.hasNext())
    {
        it.next();
        QDir dir(it.filePath());

        qCDebug(LogQtTestRunnerCore, "%s",
                dir.absolutePath().toStdString().c_str());

        QString fullPath = dir.absolutePath();

        if (isUnitTest(fullPath))
        {
            QStringList tests = getTests(fullPath);

            for (int i=0; i<m_settings->repeat; i++)
            {
                if (m_settings->onebyone)
                {
                    for (auto it=tests.begin(); it!=tests.end(); ++it)
                    {
                        const QString &test = *it;
                        m_unitTests.append(QPair<QString,QString>(fullPath, test));
                    }
                }
                else
                {
                    m_unitTests.append(QPair<QString,QString>(fullPath, ""));
                }
                emit unitTestFound(fullPath);
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
        std::sort(m_unitTests.begin(), m_unitTests.end());
    }


    int jobnr = 1;
    // Run all test executables
    for (auto it = m_unitTests.constBegin();
         !m_stopRequested && it != m_unitTests.constEnd();
         ++it)
    {
        const QString &filename = (*it).first;
        const QString &testname = (*it).second;

        qCDebug(LogQtTestRunnerCore, "Acquiring");
        sem->acquire();
        qCDebug(LogQtTestRunnerCore, "Acquired");


        UnitTestRunner *runner = new UnitTestRunner(sem);

        QObject::connect(runner, &UnitTestRunner::testSuiteChanged,
                         this, &TestManager::onTestSuiteChanged);
        QObject::connect(runner, &UnitTestRunner::endTestSuite,
                         this, &TestManager::onEndTestSuite);
        QObject::connect(runner, &UnitTestRunner::endTestCase,
                         this, &TestManager::onEndTestCase);
        QObject::connect(runner, &UnitTestRunner::endTestFunction,
                         this, &TestManager::onEndTestFunction);

        runner->start(jobnr, filename, testname);
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
