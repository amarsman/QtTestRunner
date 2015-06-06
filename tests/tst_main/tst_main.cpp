#include <QTest>
#include <QThread>

QT_USE_NAMESPACE

const int WAITTIME = 100;

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
    void passloggingtest();

    void failtest();
    void failloggingtest();

    void xfailtest();
    void xpasstest();

    void datatest_data();
    void datatest();

    void benchmarkpasstest();
    void benchmarkpassloggingtest();
private:
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
QTEST_APPLESS_MAIN(tst_main)

#include "tst_main.moc"
