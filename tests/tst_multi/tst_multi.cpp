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

const int WAITTIME = 1000;

/******************************************************************************/
class tst_application : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void passtest();
};

/******************************************************************************/
void tst_application::initTestCase()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_application::cleanupTestCase()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_application::init()
{
}

/******************************************************************************/
void tst_application::cleanup()
{
}

/******************************************************************************/
void tst_application::passtest()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
class tst_application2 : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void passtest2();
};

/******************************************************************************/
void tst_application2::initTestCase()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_application2::cleanupTestCase()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_application2::init()
{
}

/******************************************************************************/
void tst_application2::cleanup()
{
}

/******************************************************************************/
void tst_application2::passtest2()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
int main(int argc, char *argv[])
{
    JhTestSuite suite;

    suite.add<tst_application>();
    suite.add<tst_application2>();

    return suite.exec(argc, argv);
}

#include "tst_multi.moc"
