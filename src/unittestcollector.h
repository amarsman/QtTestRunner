#ifndef UNIT_TEST_RUNNER_H
#define UNIT_TEST_RUNNER_H
#include <QObject>
#include <QRunnable>
#include <QString>
#include <QStringList>
#include <QSemaphore>
#include <QSharedPointer>

/******************************************************************************/
class UnitTestCollector : public QObject, public QRunnable
{
    Q_OBJECT
public:

    UnitTestCollector();
    ~UnitTestCollector();

    void start(const QString &basepath, int nrjobs);
    void stop();

signals:
    void unittestFound(const QString &findResult);
    void collectionFinished();

protected:
    void run(void);

private:
    bool isUnitTest(const QString &filename);

    bool m_stopRequested;
    bool m_running;
    QSharedPointer<QSemaphore> sem;

    QString m_basepath;
    int m_nrjobs;
};

/******************************************************************************/

#endif // UNIT_TEST_FINDER_H

