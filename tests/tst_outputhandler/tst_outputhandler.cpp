#include <QTest>
#include <QLoggingCategory>
#include "unittestoutputhandler.h"

QT_USE_NAMESPACE

/******************************************************************************/
class tst_outputhandler : public QObject
{
    Q_OBJECT

private slots:
    void test();
private:
    void readDataFile(const QString &filename,
                      UnitTestOutputHandler &handler) const;
};

/******************************************************************************/
void tst_outputhandler::readDataFile(const QString &filename,
                                     UnitTestOutputHandler &handler) const
{
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          handler.processXmlLine(line);
       }
       inputFile.close();
    }
}

/******************************************************************************/
void tst_outputhandler::test()
{
    UnitTestOutputHandler handler;

    readDataFile("testdata.txt", handler);

    TestSuite &suite = handler.m_testSuite;

    QCOMPARE(suite.m_testCases.length(),                                            3);
    QCOMPARE(suite.m_testCases[0].m_name,                                           QString("tst_application"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions.length(),                         3);

    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_casename,                    QString("tst_application"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_name,                        QString("initTestCase"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[0].m_testfunctions[0].m_done,                        true);

    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_casename,                    QString("tst_application"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_name,                        QString("passtest"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[0].m_testfunctions[1].m_done,                        true);

    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_casename,                    QString("tst_application"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_name,                        QString("cleanupTestCase"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[0].m_testfunctions[2].m_done,                        true);
    QCOMPARE(suite.m_testCases[0].m_done,                                           true);

    QCOMPARE(suite.m_testCases[1].m_testfunctions.length(),                         3);

    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_casename,                    QString("tst_application2"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_name,                        QString("initTestCase"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[1].m_testfunctions[0].m_done,                        true);

    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_casename,                    QString("tst_application2"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_name,                        QString("passtest2"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[1].m_testfunctions[1].m_done,                        true);

    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_casename,                    QString("tst_application2"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_name,                        QString("cleanupTestCase"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[1].m_testfunctions[2].m_done,                        true);
    QCOMPARE(suite.m_testCases[1].m_done,                                           true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions.length(),                         11);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_name,                        QString("initTestCase"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[0].m_done,                        true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_name,                        QString("passtest"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[1].m_done,                        true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_name,                        QString("passloggingtest"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages.length(),           3);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[0].m_type,          QString("qdebug"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[0].m_file,          QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[0].m_line,          QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[0].m_description,   QString("Some debug"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[0].m_done,          true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[1].m_type,          QString("qwarn"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[1].m_file,          QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[1].m_line,          QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[1].m_description,   QString("Some warning"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[1].m_done,          true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[2].m_type,          QString("system"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[2].m_file,          QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[2].m_line,          QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[2].m_description,   QString("Some critical"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_messages[2].m_done,          true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[2].m_done,                        true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_name,                        QString("failtest"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_incidents[0].m_type,         QString("fail"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_incidents[0].m_file,         QString("../../../tests/tst_main/tst_main.cpp"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_incidents[0].m_line,         QString("83"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_incidents[0].m_description,  QString("Compared values are not the same\nActual   (1): 1\nExpected (0): 0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[3].m_done,                        true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_name,                        QString("failloggingtest"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages.length(),           3);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[0].m_type,          QString("qdebug"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[0].m_file,          QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[0].m_line,          QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[0].m_description,   QString("Some debug"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[0].m_done,          true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[1].m_type,          QString("qwarn"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[1].m_file,          QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[1].m_line,          QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[1].m_description,   QString("Some warning"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[1].m_done,          true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[2].m_type,          QString("system"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[2].m_file,          QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[2].m_line,          QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[2].m_description,   QString("Some critical"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_messages[2].m_done,          true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_incidents[0].m_type,         QString("fail"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_incidents[0].m_file,         QString("../../../tests/tst_main/tst_main.cpp"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_incidents[0].m_line,         QString("96"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_incidents[0].m_description,  QString("Compared values are not the same\nActual   (1): 1\nExpected (0): 0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[4].m_done,                        true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_name,                        QString("xfailtest"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents.length(),          2);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[0].m_type,         QString("xfail"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[0].m_file,         QString("../../../tests/tst_main/tst_main.cpp"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[0].m_line,         QString("103"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[1].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[1].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[1].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[1].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[1].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_incidents[1].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[5].m_done,                        true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_name,                        QString("xpasstest"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_incidents[0].m_type,         QString("xpass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_incidents[0].m_file,         QString("../../../tests/tst_main/tst_main.cpp"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_incidents[0].m_line,         QString("111"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[6].m_done,                        true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_name,                        QString("datatest"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents.length(),          3);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[0].m_datatag,      QString("positive"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[1].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[1].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[1].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[1].m_datatag,      QString("negative"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[1].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[1].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[2].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[2].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[2].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[2].m_datatag,      QString("zero"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[2].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_incidents[2].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[7].m_done,                        true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_name,                        QString("benchmarkpasstest"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[8].m_done,                        true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_name,                        QString("benchmarkpassloggingtest"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[9].m_done,                        true);

    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_casename,                    QString("tst_main"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_name,                        QString("cleanupTestCase"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_messages.length(),           0);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_incidents.length(),          1);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_incidents[0].m_type,         QString("pass"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_incidents[0].m_file,         QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_incidents[0].m_line,         QString("0"));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_incidents[0].m_datatag,      QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_incidents[0].m_description,  QString(""));
    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_incidents[0].m_done,         true);
    QCOMPARE(suite.m_testCases[2].m_testfunctions[10].m_done,                        true);
    QCOMPARE(suite.m_testCases[2].m_done,                                           true);
}

/******************************************************************************/
int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules("*.debug=false");

    tst_outputhandler tc;
    return QTest::qExec(&tc, argc, argv);
}


#include "tst_outputhandler.moc"
