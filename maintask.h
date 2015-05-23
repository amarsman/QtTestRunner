#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QLoggingCategory>

/******************************************************************************/
class MainTask : public QObject
{
    Q_OBJECT
public:
    explicit MainTask(QObject *parent = 0);
    virtual ~MainTask();

public slots:
    void run();

signals:
    void finished();

private:
    QLoggingCategory category;
};

#endif // TASK_H
