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
#ifndef UNIT_TEST_RUNNER_H
#define UNIT_TEST_RUNNER_H

#include <QObject>
#include <QRunnable>
#include <QSemaphore>
#include <QSharedPointer>

// forward
class TestSuite;
class TestCase;
class TestFunction;

/******************************************************************************/
/** \brief Runnable that runs a single test suite, test case or test function
 * and reports results by signals */
class UnitTestRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:
    /** \brief Create a test runner that clears a semaphore when done */
    UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore);
    ~UnitTestRunner();

    /** \brief Start a test */
    bool start(int a_jobNr,
               const QString &a_testSuiteName,
               const QString &a_testCaseName,
               const QString &a_testFunctionName);

    /** \brief Stop*/
    bool stop();

signals:
    /** \brief Report test function done */
    void endTestFunction (const TestFunction &a_testFunction);

    /** \brief Report test suite crashed */
    void crashTestSuite  (const TestSuite    &a_testSuite);

private:
    void run(void);

    QString m_testSuiteName;
    QString m_testCaseName;
    QString m_testFunctionName;
    QSharedPointer<QSemaphore> m_semaphore;
    bool m_running;
    bool m_stopRequested;
    int m_jobNr;
};

/******************************************************************************/

#endif // UNIT_TEST_RUNNER_H

