#ifndef TASK_H
#define TASK_H

#include <QObject>
#include "unittestfinder.h"

/******************************************************************************/
class MainTask : public QObject
{
    Q_OBJECT
public:
    explicit MainTask(QObject *parent, const QString &a_basepath, UnitTestFinder *a_finder);
    virtual ~MainTask();

public slots:
    void run();

signals:
    void finished();
private:
    QString m_basepath;
    UnitTestFinder *m_finder;
};

#endif // TASK_H
