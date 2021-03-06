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
#ifndef CONSOLE_RUNNER_H
#define CONSOLE_RUNNER_H

#include <QObject>
#include <QElapsedTimer>

#include "testsettings.h"
#include "unittestoutputhandler.h"

class TestManager; //forward
class UnitTestTriple;

/******************************************************************************/
/** \brief Class that uses the testmanager to run tests and report them in a console */
class ConsoleRunner : public QObject
{
    Q_OBJECT
public:
    /** \brief Create a console runner that uses a testmanager and testsettings */
    explicit ConsoleRunner(TestManager *a_testManager,
                           TestSettings *a_testSettings);
    virtual ~ConsoleRunner();

public slots:
    /** \brief Start the test runner */
    void onRun();

signals:
    /** \brief Test runner finished */
    void testingFinished(int exitcode);

private slots:
    /** \brief Report a new testsuite */
    void onFoundUnitTestTriple(const UnitTestTriple &a_unitTestTriple);

    /** \brief Report end test function */
    void onEndTestFunction(const TestFunction &a_testFunction, unsigned int a_testFunctionNr);

    /** \brief Report crash test suite */
    void onCrashTestSuite(const TestSuite &a_testSuite);

    /** \brief Report testing finished */
    void onFinishedTesting();

private:
    void printTestFunctionResult(const TestFunction &a_testFunction, unsigned int a_testFunctionNr);
    bool hasMessages(const TestFunction &a_testFunction);
    void printMessages(const TestFunction &a_testFunction);
    void printIncidents(const TestFunction &a_testFunction);

    TestManager *m_testManager;
    TestSettings *m_testSettings;

    unsigned int m_nrDots;
    QElapsedTimer m_elapsedTimer;
};

/******************************************************************************/

#endif // CONSOLE_RUNNER_H
