#include <QTest>
#include "unittestoutputhandler.h"

QT_USE_NAMESPACE

/******************************************************************************/
class tst_outputhandler : public QObject
{
    Q_OBJECT

private slots:
    void test();
};

/******************************************************************************/
void tst_outputhandler::test()
{
    //UnitTestOutputHandler handler;
#if 0
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          ...
       }
       inputFile.close();
    }
#endif
}

/******************************************************************************/
QTEST_APPLESS_MAIN(tst_outputhandler)

#include "tst_outputhandler.moc"
