#include "unittestmodel.h"

/******************************************************************************/
UnitTestModel::UnitTestModel()
    : QStandardItemModel()
    , m_nrrows(0)
{

}

/******************************************************************************/
UnitTestModel::~UnitTestModel()
{

}

/******************************************************************************/
void UnitTestModel::refresh(const TestSuite &a_testSuite)
{
    Locker lock(g_access);
    QStandardItem *rootItem = invisibleRootItem();

    for (auto it=a_testSuite.m_testCases.begin(); it!=a_testSuite.m_testCases.end(); ++it)
    {
        const TestCase &testcase = *it;

        QStandardItem *testcaseItem=0;

        // Check if testcase already in model, if not create it.
        {
            for (int r=0; r<rootItem->rowCount(); r++)
            {
                QStandardItem *bla = rootItem->child(r);
                if (testcase.m_name == bla->text())
                {
                    testcaseItem = bla;
                    testcaseItem->removeRows(0,testcaseItem->rowCount());
                    break;
                }
            }

            if (!testcaseItem) // not exists yet
            {
                testcaseItem = new QStandardItem(testcase.m_name);
                rootItem->appendRow(testcaseItem);
            }
        }

        const QList<TestFunction> &functions = testcase.m_testfunctions;
        for (auto it=functions.begin(); it!=functions.end(); ++it)
        {
            const TestFunction &function = *it;

            QStandardItem *functionItem = new QStandardItem(function.m_name);
            testcaseItem->appendRow(functionItem);

            bool pass = true;

            for (auto it = function.m_incidents.begin();
                    it != function.m_incidents.end();
                    ++it)
            {
                const Incident &incident = *it;
                if (!incident.m_done ||
                        incident.m_type == "fail" ||
                        incident.m_type == "xpass")
                {
                    pass = false;
                }
            }

            if (!pass)
            {
                const QList<Message> &messages = function.m_messages;
                for (auto it=messages.begin(); it!=messages.end(); ++it)
                {
                    const Message &message = *it;

                    QStandardItem *messageItem = new QStandardItem(message.m_type);
                    functionItem->appendRow(messageItem);
                }

                const QList<Incident> &incidents = function.m_incidents;
                for (auto it=incidents.begin(); it!=incidents.end(); ++it)
                {
                    const Incident &incident = *it;

                    QStandardItem *incidentItem = new QStandardItem(incident.m_type);
                    functionItem->appendRow(incidentItem);
                }
            }
        }
    }

    emit refreshDone();
}

/******************************************************************************/
