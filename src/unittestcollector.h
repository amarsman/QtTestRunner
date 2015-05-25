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

    void start(const QString &a_Path = ".");
    void stop();

signals:
    void unittestFound(const QString &findResult);
    void collectionFinished();

protected:
    void run(void);

private:
    bool isUnitTest(const QString &filename);

    QString m_searchPath;
    bool m_stopRequested;
    bool m_running;
    int m_nrjobs;
    QSharedPointer<QSemaphore> sem;
};

/******************************************************************************/

#endif // UNIT_TEST_FINDER_H

