#ifndef UNIT_TEST_FINDER_H
#define UNIT_TEST_FINDER_H
#include <QObject>
#include <QRunnable>
#include <QString>
#include <QSemaphore>
#include <QSharedPointer>
#include <QRegularExpression>

/******************************************************************************/
class UnitTestRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:

    UnitTestRunner(QSharedPointer<QSemaphore> a_semaphore);
    ~UnitTestRunner();

    void start(int jobnr, const QString &a_unitTest);
    void stop();

signals:
    void unitTestResult(int jobnr,
                        const QString &testCase,
                        const QString &testFunction,
                        const QString &result);

protected:
    void run(void);

private:
    void processXmlLine(const QString &line);
    QString m_unitTest;
    QSharedPointer<QSemaphore> m_semaphore;
    bool m_running;
    bool m_stopRequested;
    int m_jobnr;

    QRegularExpression re_tc_start;
    QRegularExpression re_tc_end;
    QRegularExpression re_environment_start;
    QRegularExpression re_environment_end;
    QRegularExpression re_tf_start;
    QRegularExpression re_tf_end;
    QRegularExpression re_incident;
    QRegularExpression re_incident_start;
    QRegularExpression re_incident_end;
    QRegularExpression re_duration;
    bool m_inTestcase;
    QString m_testCaseName;
    bool m_inEnvironment;
    bool m_inTestFunction;
    QString m_testFunctionName;
    bool m_inIncident;
};

/******************************************************************************/

#endif // UNIT_TEST_FINDER_H

