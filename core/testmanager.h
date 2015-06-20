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
#ifndef TEST_MANAGER_H
#define TEST_MANAGER_H

#include <QObject>
#include <QRunnable>
#include <QSemaphore>
#include <QSharedPointer>
#include <QString>

#include "testsettings.h"
#include "unittestoutputhandler.h"

/******************************************************************************/
/** \brief Struct that holds a testable */
class UnitTestTriple
{
public:
    UnitTestTriple()
    {
        reset();
    }
    /** \brief reset data */
    void reset()
    {
        m_testSuiteName = "";
        m_testCaseName = "";
        m_testFunctionName = "";
    }

    QString m_testSuiteName;     /*!< \brief Name of test suite */
    QString m_testCaseName;      /*!< \brief Name of test case */
    QString m_testFunctionName;  /*!< \brief Name of test function */
};

/******************************************************************************/
/** \brief Runnable that collects testables and runs them multi threaded */
class TestManager : public QObject, public QRunnable
{
    Q_OBJECT
public:

    TestManager();
    ~TestManager();

    bool isRunning();

public slots:
    /** \brief Start the test manager */
    void start(TestSettings *a_testSettings);

    /** \brief Stop the test manager */
    void stop();

    /** \brief Get nr of found test functions */
    unsigned int getNrFoundTestFunctions();
    unsigned int getNrRunTestFunctions();
    unsigned int getNrPassedFunctions();
    unsigned int getNrFailedTestFunctions();

signals:
    /** \brief Report a new test suite */
    void foundUnitTestTriple(const UnitTestTriple  &a_unitTestTriple);

    /** \brief Report test function done */
    void endTestFunction(const TestFunction &a_testFunction, unsigned int m_testFunctionNr);

    /** \brief Report test suite crashed */
    void crashTestSuite(const TestSuite &a_testSuiteName);

    /** \brief Report testing finished */
    void finishedTesting();


private slots:
    void onEndTestFunction(const TestFunction &a_testFunction);
    void onCrashTestSuite(const TestSuite &a_testSuiteName);

private:
    void run(void);
    bool isTestSuite(const QString &a_fileName);
    bool getTests(const QString &a_testSuiteName,
                  QList<UnitTestTriple> &a_testTriples,
                  unsigned int &a_nrTests);

    TestSettings *m_testSettings;
    bool m_stopRequested;
    bool m_running;
    QSharedPointer<QSemaphore> sem;
    unsigned int m_nrFoundTestFunctions;
    unsigned int m_nrRunTestFunctions;
    unsigned int m_nrPassedTestFunctions;
    unsigned int m_nrFailedTestFunctions;
};

/******************************************************************************/

#endif // TEST_MANAGER_H

