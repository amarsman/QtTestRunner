/*
Copyright (C) 2015 Henk van der Laak

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
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
    : m_testSettings(nullptr)
    , m_stopRequested(false)
    , m_running(false)
    , m_nrFoundTestFunctions(0)
    , m_nrRunTestFunctions(0)
    , m_nrPassedTestFunctions(0)
    , m_nrFailedTestFunctions(0)
{
    qRegisterMetaType<UnitTestTriple>("UnitTestTriple");
    qCDebug(LogQtTestRunnerCore);
    setAutoDelete(false);
}

/******************************************************************************/
TestManager::~TestManager()
{
    qCDebug(LogQtTestRunnerCore);
}

/******************************************************************************/
bool TestManager::isRunning()
{
    return m_running;
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
unsigned int TestManager::getNrFoundTestFunctions()
{
    qCDebug(LogQtTestRunnerCore);
    return m_nrFoundTestFunctions * m_testSettings->repeat;
}

/******************************************************************************/
unsigned int TestManager::getNrRunTestFunctions()
{
    qCDebug(LogQtTestRunnerCore);
    return m_nrRunTestFunctions;
}

/******************************************************************************/
unsigned int TestManager::getNrPassedFunctions()
{
    qCDebug(LogQtTestRunnerCore);
    return m_nrPassedTestFunctions;
}

/******************************************************************************/
unsigned int TestManager::getNrFailedTestFunctions()
{
    qCDebug(LogQtTestRunnerCore);
    return m_nrFailedTestFunctions;
}

/******************************************************************************/
void TestManager::onEndTestFunction(const TestFunction &a_testFunction)
{
    qCDebug(LogQtTestRunnerCore);

    // Do not process init/cleanup
    if (a_testFunction.m_name == "initTestCase") return;
    if (a_testFunction.m_name == "cleanupTestCase") return;

    m_nrRunTestFunctions++;
    if (a_testFunction.m_pass)
    {
        m_nrPassedTestFunctions++;
    }
    else
    {
        m_nrFailedTestFunctions++;
    }
    emit endTestFunction(a_testFunction, m_nrRunTestFunctions);
}

/******************************************************************************/
void TestManager::onCrashTestSuite(const TestSuite &a_testSuiteName)
{
    qCDebug(LogQtTestRunnerCore);
    emit crashTestSuite(a_testSuiteName);
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
                if (!getTests(fullPath, tests, nrtests))
                {
                    emit finishedTesting();
                    return;
                }
                m_unitTests.append(tests);
                m_nrFoundTestFunctions += nrtests;
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
            if (!getTests(fullPath, tests, nrtests))
            {
                emit finishedTesting();
                return;
            }
            m_unitTests.append(tests);
            m_nrFoundTestFunctions += nrtests;
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
                         this, &TestManager::onEndTestFunction);
        QObject::connect(runner, &UnitTestRunner::crashTestSuite,
                         this, &TestManager::onCrashTestSuite);

        runner->start(jobnr, filename, testcase, testname);
    }

    while (sem->available() != m_testSettings->nrjobs)
    {
        qCDebug(LogQtTestRunnerCore, "Waiting for %d runners to finish",
                m_testSettings->nrjobs - sem->available());
        QThread::msleep(1000);
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
    if (!process->waitForFinished(5000) ||
            process->exitStatus() != QProcess::NormalExit ||
            process->exitCode() != 0)
    {
        //qCCritical(LogQtTestRunnerCore, "Could not analyze %s", a_fileName.toLatin1().data());
        return false;
    }
    QString data = QString(process->readAllStandardOutput());

    QRegularExpression re("libQt5Test");

    return (re.match(data).hasMatch());
}

/******************************************************************************/
bool TestManager::getTests(const QString &a_testSuiteFileName,
                           QList<UnitTestTriple> &a_testTriples,
                           unsigned int &a_nrTests)
{
    a_testTriples.clear();
    a_nrTests = 0;

    QString filename = QFileInfo(a_testSuiteFileName).absoluteFilePath();

    if (!m_testSettings->jhextensions) // don't use JH extensions
    {
        QScopedPointer<QProcess> process(new QProcess());
        process->start(filename, QStringList() << "-functions");
        if (!process->waitForFinished(5000) ||
                process->exitStatus() != QProcess::NormalExit ||
                process->exitCode() != 0)
        {
            qCCritical(LogQtTestRunnerCore, "Could not analyze %s",
                       a_testSuiteFileName.toLatin1().data());
            return false;
        }

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
                    emit foundUnitTestTriple(triple);
                }
                a_nrTests++;
            }
        }
        if (!m_testSettings->isolated)
        {
            UnitTestTriple triple;
            triple.m_testSuiteName = filename;
            triple.m_testCaseName = "";
            triple.m_testFunctionName = "";
            a_testTriples.append(triple);
            emit foundUnitTestTriple(triple);
        }
    }
    else // use JH extensions
    {
        QStringList testcases;

        QScopedPointer<QProcess> process(new QProcess());
        process->start(filename, QStringList() << "-testcases");
        if (!process->waitForFinished(5000) ||
                process->exitStatus() != QProcess::NormalExit ||
                process->exitCode() != 0)
        {
            qCCritical(LogQtTestRunnerCore, "Could not analyze %s",
                       a_testSuiteFileName.toLatin1().data());
            return false;
        }

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
            if (!process->waitForFinished(5000) ||
                    process->exitStatus() != QProcess::NormalExit ||
                    process->exitCode() != 0)
            {
                qCCritical(LogQtTestRunnerCore, "Could not analyze %s",
                           a_testSuiteFileName.toLatin1().data());
                return false;
            }

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
                        emit foundUnitTestTriple(triple);
                    }
                    a_nrTests++;
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
            emit foundUnitTestTriple(triple);
        }

    }

    return true;
}

/******************************************************************************/
