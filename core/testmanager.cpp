#include <QDirIterator>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QProcess>
#include <QThreadPool>

#include "testmanager.h"
#include "unittestrunner.h"


Q_LOGGING_CATEGORY(LogQtTestRunnerCore, "QtTestRunnerCore")

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
void TestManager::start(TestSettings *a_testSettings)
{
    qCDebug(LogQtTestRunnerCore);
    if (m_running)
    {
        return;
    }
    m_testSettings = a_testSettings;
    sem.reset(new QSemaphore(m_testSettings->nrjobs));

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
void TestManager::run()
{
    qCDebug(LogQtTestRunnerCore, "Starting");

    if (m_running)
    {
        qCDebug(LogQtTestRunnerCore, "Finished");
        emit finishedTesting();
        return;
    }

    m_running = true;

    // Collect all tests executables
    QList<UnitTestTriple> m_unitTests;

    if (QFileInfo(m_testSettings->basepath).isDir())
    {
        QDirIterator it(m_testSettings->basepath,
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
            }
        }
    }
    else if (QFileInfo(m_testSettings->basepath).isFile())
    {
        QDir dir(m_testSettings->basepath);
        qCDebug(LogQtTestRunnerCore, "%s",
                dir.absolutePath().toLatin1().data());

        QString fullPath = dir.absolutePath();

        if (isTestSuite(fullPath))
        {
            QList<UnitTestTriple> tests;
            unsigned int nrtests=0;
            getTests(fullPath, tests, nrtests);
            m_unitTests.append(tests);
        }
    }

    QList<UnitTestTriple> m_unitTests_old = m_unitTests;

    for (int i=1; i<m_testSettings->repeat; i++)
    {
        m_unitTests.append(m_unitTests_old);
    }

    qCDebug(LogQtTestRunnerCore, "Sorting");
    if (m_testSettings->shuffle)
    {
        std::random_shuffle(m_unitTests.begin(), m_unitTests.end());
    }

    int jobnr = 0;
    // Run all test executables
    for (auto it = m_unitTests.constBegin();
            !m_stopRequested && it != m_unitTests.constEnd();
            ++it)
    {
        jobnr++;

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
                         this, &TestManager::endTestFunction);
        QObject::connect(runner, &UnitTestRunner::crashTestSuite,
                         this, &TestManager::crashTestSuite);

        runner->start(jobnr, filename, testcase, testname);
    }

    qCDebug(LogQtTestRunnerCore, "Waiting for runners to finish");
    while (sem->available() != m_testSettings->nrjobs)
    {
        QThread::msleep(100);
    }

    m_running = false;
    qCDebug(LogQtTestRunnerCore, "Finished");
    emit finishedTesting();
}

/******************************************************************************/
bool TestManager::isTestSuite(const QString &a_fileName)
{
    QScopedPointer<QProcess> process(new QProcess());

    process->start("ldd",QStringList() << a_fileName);
    process->waitForFinished();
    QString data = QString(process->readAllStandardOutput());

    QRegularExpression re("libQt5Test");

    return (re.match(data).hasMatch());
}

/******************************************************************************/
void TestManager::getTests(const QString &a_testSuiteName,
                           QList<UnitTestTriple> &a_testTriples,
                           unsigned int &a_nrTests)
{
    a_testTriples.clear();
    a_nrTests = 0;

    QString filename = QFileInfo(a_testSuiteName).absoluteFilePath();

    if (!m_testSettings->jhextensions) // don't use JH extensions
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
                if (m_testSettings->isolated)
                {
                    UnitTestTriple triple;
                    triple.m_testSuiteName = filename;
                    triple.m_testCaseName = "";
                    triple.m_testFunctionName = line.left(line.length()-2);
                    a_testTriples.append(triple);
                }
                a_nrTests++;
                emit foundTestSuite(a_testSuiteName, 1);
            }
        }
        if (!m_testSettings->isolated)
        {
            UnitTestTriple triple;
            triple.m_testSuiteName = filename;
            triple.m_testCaseName = "";
            triple.m_testFunctionName = "";
            a_testTriples.append(triple);
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
                    if (m_testSettings->isolated)
                    {
                        UnitTestTriple triple;
                        triple.m_testSuiteName = filename;
                        triple.m_testCaseName = testcase;
                        triple.m_testFunctionName = line.left(line.length()-2);
                        a_testTriples.append(triple);
                    }
                    a_nrTests++;
                    emit foundTestSuite(a_testSuiteName, 1);
                }
            }
        }

        if (!m_testSettings->isolated)
        {
            UnitTestTriple triple;
            triple.m_testSuiteName = filename;
            triple.m_testCaseName = "";
            triple.m_testFunctionName = "";
            a_testTriples.append(triple);
        }
    }
}

/******************************************************************************/
