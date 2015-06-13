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
bool TestManager::isTestSuite(const QString &filename)
{
    QScopedPointer<QProcess> process(new QProcess());

    process->start("ldd",QStringList() << filename);
    process->waitForFinished();
    QString data = QString(process->readAllStandardOutput());

    QRegularExpression re("libQt5Test");

    return (re.match(data).hasMatch());
}

/******************************************************************************/
void TestManager::getTests(const QString &a_testSuite, QList<UnitTestTriple> &a_testList, unsigned int &a_nrTests)
{
    a_testList.clear();
    a_nrTests = 0;

    QString filename = QFileInfo(a_testSuite).absoluteFilePath();

    if (!m_settings->jhextensions) // don't use JH extensions
    {
        QScopedPointer<QProcess> process(new QProcess());
        process->start(filename, QStringList() << "-functions");
        process->waitForFinished();

        QStringList data = QString(process->readAllStandardOutput()).split('\n');

        for (auto it = data.begin(); it != data.end(); ++it)
        {
            QString &line = *it;

            if (line.endsWith("()"))
            {
                if (m_settings->isolated)
                {
                    UnitTestTriple triple;
                    triple.m_testSuiteName = filename;
                    triple.m_testCaseName = "";
                    triple.m_testFunctionName = line.left(line.length()-2);
                    a_testList.append(triple);
                }
                a_nrTests++;
                emit foundTestSuite(a_testSuite, 1);
            }
        }
        if (!m_settings->isolated)
        {
            UnitTestTriple triple;
            triple.m_testSuiteName = filename;
            triple.m_testCaseName = "";
            triple.m_testFunctionName = "";
            a_testList.append(triple);
        }
    }
    else // use JH extensions
    {
        QStringList testcases;

        QScopedPointer<QProcess> process(new QProcess());
        process->start(filename, QStringList() << "-testcases");
        process->waitForFinished();

        QStringList data = QString(process->readAllStandardOutput()).split('\n');
        for (auto it = data.begin(); it != data.end(); ++it)
        {
            QString line = (*it).trimmed();
            if (line.startsWith("- "))
            {
                testcases.append(line.right(line.length()-2));
            }
        }

        for (auto it = testcases.begin(); it != testcases.end(); ++it)
        {
            QString testcase = (*it).trimmed();
            QScopedPointer<QProcess> process(new QProcess());
            process->start(filename, QStringList() << "-testcase" << testcase << "-functions" );
            process->waitForFinished();

            QStringList data = QString(process->readAllStandardOutput()).split('\n');

            for (auto it = data.begin(); it != data.end(); ++it)
            {
                QString &line = *it;

                if (line.endsWith("()"))
                {
                    if (m_settings->isolated)
                    {
                        UnitTestTriple triple;
                        triple.m_testSuiteName = filename;
                        triple.m_testCaseName = testcase;
                        triple.m_testFunctionName = line.left(line.length()-2);
                        a_testList.append(triple);
                    }
                    a_nrTests++;
                    emit foundTestSuite(a_testSuite, 1);
                }
            }
        }

        if (!m_settings->isolated)
        {
            UnitTestTriple triple;
            triple.m_testSuiteName = filename;
            triple.m_testCaseName = "";
            triple.m_testFunctionName = "";
            a_testList.append(triple);
        }
    }
}

/******************************************************************************/
void TestManager::onEndTestFunction(const TestFunction &a_testFunction)
{
    emit endTestFunction(a_testFunction);
}

/******************************************************************************/
void TestManager::onCrashTestSuite(const TestSuite &a_testSuite)
{
    emit crashTestSuite(a_testSuite);
}

/******************************************************************************/
void TestManager::run()
{
    qCDebug(LogQtTestRunnerCore, "Starting");

    if (m_running)
    {
        qCDebug(LogQtTestRunnerCore, "Finished");
        emit testingFinished();
        return;
    }

    m_running = true;

    // Collect all tests executables
    QList<UnitTestTriple> m_unitTests;

    if (QFileInfo(m_settings->basepath).isDir())
    {
        QDirIterator it(m_settings->basepath,
                        QStringList() << "*",
                        QDir::Files | QDir::Executable,
                        QDirIterator::Subdirectories);

        while (!m_stopRequested && it.hasNext())
        {
            it.next();
            QDir dir(it.filePath());

            qCDebug(LogQtTestRunnerCore, "%s",
                    dir.absolutePath().toLatin1().data());

            QString fullPath = dir.absolutePath();

            if (isTestSuite(fullPath))
            {
                QList<UnitTestTriple> tests;
                unsigned int nrtests=0;
                getTests(fullPath, tests, nrtests);
                m_unitTests.append(tests);

                //emit unitTestFound(fullPath, nrtests);
            }
        }
    }
    else if (QFileInfo(m_settings->basepath).isFile())
    {
        QDir dir(m_settings->basepath);
        qCDebug(LogQtTestRunnerCore, "%s",
                dir.absolutePath().toLatin1().data());

        QString fullPath = dir.absolutePath();

        if (isTestSuite(fullPath))
        {
            QList<UnitTestTriple> tests;
            unsigned int nrtests=0;
            getTests(fullPath, tests, nrtests);
            m_unitTests.append(tests);

            //emit unitTestFound(fullPath, nrtests);
        }
    }

    QList<UnitTestTriple> m_unitTests_old = m_unitTests;

    for (int i=1; i<m_settings->repeat; i++)
    {
        m_unitTests.append(m_unitTests_old);
    }

    qCDebug(LogQtTestRunnerCore, "Sorting");
    if (m_settings->shuffle)
    {
        std::random_shuffle(m_unitTests.begin(), m_unitTests.end());
    }

    int jobnr = 1;
    // Run all test executables
    for (auto it = m_unitTests.constBegin();
            !m_stopRequested && it != m_unitTests.constEnd();
            ++it)
    {
        const QString &filename = (*it).m_testSuiteName;
        const QString &testcase = (*it).m_testCaseName;
        const QString &testname = (*it).m_testFunctionName;

        qCDebug(LogQtTestRunnerCore, "Acquiring");
        if (!sem->tryAcquire(1, 180000)) // try for three minutes
        {
            qCDebug(LogQtTestRunnerCore, "Acquire failed");
            continue;
        }
        qCDebug(LogQtTestRunnerCore, "Acquired");


        UnitTestRunner *runner = new UnitTestRunner(sem);

        QObject::connect(runner, &UnitTestRunner::endTestFunction,
                         this, &TestManager::onEndTestFunction);
        QObject::connect(runner, &UnitTestRunner::crashTestSuite,
                         this, &TestManager::onCrashTestSuite);

        runner->start(jobnr, filename, testcase, testname);
        jobnr++;
    }

    qCDebug(LogQtTestRunnerCore, "Waiting for runners to finish");
    while (sem->available() != m_settings->nrjobs)
    {
        QThread::msleep(100);
    }

    m_running = false;
    qCDebug(LogQtTestRunnerCore, "Finished");
    emit testingFinished();
}

/******************************************************************************/
