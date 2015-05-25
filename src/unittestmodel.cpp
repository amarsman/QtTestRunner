#include "unittestmodel.h"

/******************************************************************************/
UnitTestModel::UnitTestModel(QObject *parent) : QAbstractItemModel(parent)
{

}

/******************************************************************************/
void UnitTestModel::clear()
{
    beginResetModel();
    removeRows(0, rowCount());
    endResetModel();
}

/******************************************************************************/
QModelIndex UnitTestModel::index(int row, int column, const QModelIndex & parent) const
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    return QAbstractItemModel::createIndex(row, column);
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
    return 5;
}

/******************************************************************************/
int UnitTestModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 5;
}

/******************************************************************************/
QVariant UnitTestModel::data(const QModelIndex & index, int role) const
{
    Q_UNUSED(index);

    if (role == Qt::DisplayRole)
    {
        return QString("hai");
    }
    return QVariant();
}

/******************************************************************************/
