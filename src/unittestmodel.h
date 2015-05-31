#ifndef UNITTESTMODEL_H
#define UNITTESTMODEL_H
#include <QAbstractItemModel>
#include "unittestoutputhandler.h"

class UnitTestModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    UnitTestModel();
    ~UnitTestModel();

    void refresh(const TestCase &a_testcase);

    virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex & index) const;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private:
    TestCase m_testcase;
    int m_nrrows;
};

#endif // UNITTESTMODEL_H
