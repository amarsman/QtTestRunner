#ifndef UNIT_TEST_FINDER_H
#define UNIT_TEST_FINDER_H
#include <QObject>
#include <QRunnable>
#include <QString>

/******************************************************************************/
class UnitTestFinder : public QObject, public QRunnable
{
    Q_OBJECT
public:

    UnitTestFinder();
    ~UnitTestFinder();

    void start(const QString &a_TextPattern,
                           const QString &a_Path = ".",
                           const QString &a_FilePattern = "*.cpp");
    void stop();

signals:
    void resultFound(const QString &findResult);
    void finished();

protected:
    void run(void);

private:
    QString m_TextPattern;
    QString m_SearchPath;
    QString m_FilePattern;
    bool m_StopRequested;
    bool m_Running;
};

/******************************************************************************/

#endif // UNIT_TEST_FINDER_H

