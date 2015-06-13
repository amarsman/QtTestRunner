#include <QTest>
#include <QThread>
#include "../jhtestsuite.h"

const int WAITTIME = 0;

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
