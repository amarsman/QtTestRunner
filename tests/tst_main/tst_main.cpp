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
#include <QTest>
#include <QThread>
#include "../jhtestsuite.h"

QT_USE_NAMESPACE

const int WAITTIME = 0;

/******************************************************************************/
class tst_main : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void passtest();
    void xfailtest();
    void datatest_data();
    void datatest();

    void benchmarkpasstest();
    void benchmarkpassloggingtest();

    void testvector_data();
    void testvector();

    void passloggingtest();
    void failloggingtest();
    void failtest();
    void xpasstest();

    void testcrash();
};

/******************************************************************************/
void tst_main::initTestCase()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_main::cleanupTestCase()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_main::init()
{
}

/******************************************************************************/
void tst_main::cleanup()
{
}

/******************************************************************************/
void tst_main::passtest()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_main::passloggingtest()
{
    QThread::msleep(WAITTIME);
    qDebug("Some debug");

    QThread::msleep(WAITTIME);
    qWarning("Some warning");

    QThread::msleep(WAITTIME);
    qCritical("Some critical");
}

/******************************************************************************/
void tst_main::failtest()
{
    QThread::msleep(WAITTIME);

    QCOMPARE(1,0);
}

/******************************************************************************/
void tst_main::failloggingtest()
{
    QThread::msleep(WAITTIME);
    qDebug("Some debug");

    QThread::msleep(WAITTIME);
    qWarning("Some warning");

    QThread::msleep(WAITTIME);
    qCritical("Some critical");

    QThread::msleep(WAITTIME);
    QCOMPARE(1,0);
}

/******************************************************************************/
void tst_main::xfailtest()
{
    QThread::msleep(WAITTIME);

    QEXPECT_FAIL("","This will go wrong", Continue);
    QCOMPARE(1,0);
}

/******************************************************************************/
void tst_main::xpasstest()
{
    QThread::msleep(WAITTIME);

    QEXPECT_FAIL("","This will go right", Continue);
    QCOMPARE(0,0);
}

/******************************************************************************/
void tst_main::datatest_data()
{
    QTest::addColumn<QString>("aString");
    QTest::addColumn<int>("expected");

    QTest::newRow("positive") << "42" << 42;
    QTest::newRow("negative") << "-42" << -42;
    QTest::newRow("zero") << "0" << 0;
}

/******************************************************************************/
void tst_main::datatest()
{
    QThread::msleep(WAITTIME);

    QFETCH(QString, aString);
    QFETCH(int, expected);
    QCOMPARE(aString.toInt(), expected);
}

/******************************************************************************/
void tst_main::benchmarkpasstest()
{
    QString a("42");
    QBENCHMARK
    {
        QCOMPARE(a.toInt(), 42);
    }
}

/******************************************************************************/
void tst_main::benchmarkpassloggingtest()
{
    QString a("42");
    //qDebug("Some debug");
    //qWarning("Some warning");
    //qCritical("Some critical");

    QBENCHMARK
    {
        QCOMPARE(a.toInt(), 42);
    }
}

/******************************************************************************/
void tst_main::testcrash()
{
    int *a = 0;
    *a = 0;
}

/******************************************************************************/
void tst_main::testvector_data()
{
    QTest::addColumn<int>("anint");
    QTest::newRow("set1") << 1;
    QTest::newRow("set2") << 0;
    QTest::newRow("set3") << 0;
    QTest::newRow("set4") << 1;
}

/******************************************************************************/
void tst_main::testvector()
{
    QFETCH(int, anint);

    QCOMPARE(anint, 0);
}

/******************************************************************************/
int main(int argc, char *argv[])
{
    JhTestSuite suite;

    suite.add<tst_main>();

    return suite.exec(argc, argv);
}

#include "tst_main.moc"
