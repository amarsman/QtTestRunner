#include "unittestmodel.h"

/******************************************************************************/
UnitTestModel::UnitTestModel() : QStandardItemModel(), m_nrrows(0)
{

}

/******************************************************************************/
UnitTestModel::~UnitTestModel()
{

}

/******************************************************************************/
void UnitTestModel::refresh(const TestSuite &a_testCase)
{
    Q_UNUSED(a_testCase);
    clear();
#if 0
    QStandardItem *rootItem = invisibleRootItem();
    QStandardItem *testcaseItem = new QStandardItem(a_testcase.m_name);
    rootItem->appendRow(testcaseItem);

    const QList<TestFunction> &functions = a_testcase.m_testfunctions;
    for (auto it=functions.begin(); it!=functions.end(); ++it)
    {
        const TestFunction &function = *it;

        QStandardItem *functionItem = new QStandardItem(function.m_name);
        testcaseItem->appendRow(functionItem);

        bool pass = true;

        for (auto it = function.m_incidents.begin(); it != function.m_incidents.end(); ++it)
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

#endif
    emit refreshDone();
}
/******************************************************************************/
