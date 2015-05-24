#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QScopedPointer>

class UnitTestCollector; //forward

/******************************************************************************/
class MainTask : public QObject
{
    Q_OBJECT
public:
    explicit MainTask(QObject *parent, const QString &a_basepath);
    virtual ~MainTask();

public slots:
    void run();
    void onUnitTestFound(const QString &a_path);
    void onCollectionFinished();

signals:
    void finished();

private:
    void startCollecting();

    QString m_basepath;
    QScopedPointer<UnitTestCollector> m_unitTestCollector;
};

#endif // MAIN_TASK_H
