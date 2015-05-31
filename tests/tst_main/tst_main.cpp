#include <QTest>
#include <QThread>

QT_USE_NAMESPACE

const int WAITTIME = 50;

/******************************************************************************/
class tst_main : public QObject
{
    Q_OBJECT

private slots:
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
    QThread::msleep(WAITTIME);
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
    QEXPECT_FAIL("","This will go wrong", Continue);
    QCOMPARE(1,0);

}

/******************************************************************************/
void tst_main::xpasstest()
{
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
    QTest::newRow("zero") << "0" << 1;
}

/******************************************************************************/
void tst_main::datatest()
{
    QFETCH(QString, aString);
    QFETCH(int, expected);
    QThread::msleep(WAITTIME);
    QCOMPARE(aString.toInt(), expected);
    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_main::benchmarkpasstest()
{
    QThread::msleep(WAITTIME);

    QString a("42");
    QBENCHMARK
    {
        QCOMPARE(a.toInt(), 42);
    }

    QThread::msleep(WAITTIME);
}

/******************************************************************************/
void tst_main::benchmarkpassloggingtest()
{
    QThread::msleep(WAITTIME);

    QString a("42");
    //qDebug("Some debug");
    //qWarning("Some warning");
    //qCritical("Some critical");

    QBENCHMARK
    {
        QCOMPARE(a.toInt(), 42);
    }

    QThread::msleep(WAITTIME);
}


/******************************************************************************/
QTEST_APPLESS_MAIN(tst_main)

#include "tst_main.moc"
