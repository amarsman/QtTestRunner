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
QList<TestTriple> TestManager::getTests(const QString &a_unittest)
{
    QList<TestTriple> tests;

    QString filename = QFileInfo(a_unittest).absoluteFilePath();

    if (!m_settings->isolated) // run unittest as a whole
    {
        TestTriple triple;
        triple.m_testunit = filename;
        triple.m_testcase = "";
        triple.m_testname = "";
        tests.append(triple);
    }
    else
    {
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
                    TestTriple triple;
                    triple.m_testunit = filename;
                    triple.m_testcase = "";
                    triple.m_testname = line.left(line.length()-2);
                    tests.append(triple);
                    //emit unitTestFound(test);
                }
            }
        }
        else // use JH extensions
        {
            QStringList testcases;

            QScopedPointer<QProcess> process(new QProcess());
            process->start(filename, QStringList() << "-testcases");
            process->waitForFinished();

            QStringList data = QString(process->readAllStandardError()).split('\n');
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
                        TestTriple triple;
                        triple.m_testunit = filename;
                        triple.m_testcase = testcase;
                        triple.m_testname = line.left(line.length()-2);
                        tests.append(triple);
                        //emit unitTestFound(test);
                    }
                }

            }
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
    QList<TestTriple> m_unitTests;

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
                dir.absolutePath().toStdString().c_str());

        QString fullPath = dir.absolutePath();

        if (isUnitTest(fullPath))
        {
                m_unitTests.append(getTests(fullPath));
            }
        }
    }
    else if (QFileInfo(m_settings->basepath).isFile())
    {
        QDir dir(m_settings->basepath);
        qCDebug(LogQtTestRunnerCore, "%s",
                dir.absolutePath().toStdString().c_str());

        QString fullPath = dir.absolutePath();

        if (isUnitTest(fullPath))
            {
            m_unitTests.append(getTests(fullPath));

                    }
                }

    QList<TestTriple> m_unitTests_old = m_unitTests;

    for (int i=1; i<m_settings->repeat; i++)
                {
        m_unitTests.append(m_unitTests_old);
                }

    qCDebug(LogQtTestRunnerCore, "Sorting");
    if (m_settings->shuffle)
    {
        std::random_shuffle(m_unitTests.begin(), m_unitTests.end());
    }
    else
    {
        //std::sort(m_unitTests.begin(), m_unitTests.end());
    }


    int jobnr = 1;
    // Run all test executables
    for (auto it = m_unitTests.constBegin();
         !m_stopRequested && it != m_unitTests.constEnd();
         ++it)
    {
        const QString &filename = (*it).m_testunit;
        const QString &testcase = (*it).m_testcase;
        const QString &testname = (*it).m_testname;

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
    emit finished();
}

/******************************************************************************/
