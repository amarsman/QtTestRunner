#include "unittestmodel.h"

/******************************************************************************/
UnitTestModel::UnitTestModel() : QAbstractItemModel(), m_nrrows(0)
{

}

/******************************************************************************/
UnitTestModel::~UnitTestModel()
{

}

/******************************************************************************/
void UnitTestModel::refresh(const TestCase &a_testcase)
{
    m_testcase = a_testcase;

    Q_UNUSED(a_testcase);
    fprintf(stderr,"%s\n", a_testcase.m_name.toStdString().c_str());

    beginResetModel();

    m_nrrows = a_testcase.m_testfunctions.count();

    endResetModel();
}

/******************************************************************************/
QModelIndex UnitTestModel::index(int row, int column, const QModelIndex & parent) const
{
    if (!parent.isValid()) // top level
    {
        return createIndex(row, column);
    }
    return QModelIndex();
}

/******************************************************************************/
QModelIndex UnitTestModel::parent(const QModelIndex & index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

/******************************************************************************/
int UnitTestModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_nrrows;
}

/******************************************************************************/
int UnitTestModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 1;
}

/******************************************************************************/
QVariant UnitTestModel::data(const QModelIndex & index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);
    switch (role)
    {
    case Qt::DisplayRole:
    {
        int rownr = index.row();
        QString name = m_testcase.m_testfunctions[rownr].m_name;

        return QVariant(name);
    }
    }

    return QVariant();
}

/******************************************************************************/
