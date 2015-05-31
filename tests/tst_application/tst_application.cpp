#include <QTest>
#include <QThread>

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
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_application::cleanup()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_application::passtest()
{
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
QTEST_APPLESS_MAIN(tst_application)

#include "tst_application.moc"
