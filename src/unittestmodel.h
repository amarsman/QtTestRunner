#ifndef UNITTESTMODEL_H
#define UNITTESTMODEL_H

#include <QAbstractItemModel>

class UnitTestModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit UnitTestModel(QObject *parent = 0);

    void clear();

protected:
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex & index) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
};

#endif // UNITTESTMODEL_H
