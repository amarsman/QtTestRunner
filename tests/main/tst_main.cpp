#include <QtTest/QtTest>

QT_USE_NAMESPACE

/******************************************************************************/
class tst_main : public QObject
{
    Q_OBJECT

private slots:
    void myTest();
};

/******************************************************************************/
void tst_main::myTest()
{

}

/******************************************************************************/
QTEST_APPLESS_MAIN(tst_main)
#include "tst_main.moc"
