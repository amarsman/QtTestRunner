#include <QtTest/QtTest>

QT_USE_NAMESPACE

/******************************************************************************/
class tst_main : public QObject
{
    Q_OBJECT

private slots:
    void myTest1();
    void myTest2();
    void myTest3_data();
    void myTest3();
    void myTest4();
private:
};

/******************************************************************************/
void tst_main::myTest1()
{
    qDebug("Some debug");
    qWarning("Some warning");
    qCritical("Some critical");
}

/******************************************************************************/
void tst_main::myTest2()
{
    QCOMPARE(1,0);
}

/******************************************************************************/
void tst_main::myTest3_data()
{
    QTest::addColumn<QString>("aString");
    QTest::addColumn<int>("expected");

    QTest::newRow("positive") << "42" << 42;
    QTest::newRow("negative") << "-42" << -42;
    QTest::newRow("zero") << "+0" << 1;
}

/******************************************************************************/
void tst_main::myTest3()
{
    QFETCH(QString, aString);
    QFETCH(int, expected);
    QCOMPARE(aString.toInt(), expected);
}

/******************************************************************************/
void tst_main::myTest4()
{
    QString a("42");
    QBENCHMARK
    {
        QCOMPARE(a.toInt(), 42);
    }
}

/******************************************************************************/
QTEST_APPLESS_MAIN(tst_main)

#include "tst_main.moc"
