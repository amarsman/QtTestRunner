#include <QTest>

/******************************************************************************/
class tst_application : public QObject
{
    Q_OBJECT

private slots:
    void passtest();
};

/******************************************************************************/
void tst_application::passtest()
{
}

/******************************************************************************/
QTEST_APPLESS_MAIN(tst_application)

#include "tst_application.moc"
