#include <QString>
#include <QRegularExpression>

#include "maintask.h"
#include "logging.h"
#include "unittestcollector.h"


/******************************************************************************/
MainTask::MainTask(QObject *parent, const QString &a_basepath, int a_nrjobs)
    : QObject(parent)
    , m_basepath(a_basepath)
    , m_nrjobs(a_nrjobs)
{
    qCDebug(LogQtTestRunner);
    m_unitTestCollector.reset(new UnitTestCollector());
}


/******************************************************************************/
MainTask::~MainTask()
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
void MainTask::run()
{
    qCDebug(LogQtTestRunner);

    startCollecting(m_basepath, m_nrjobs);
}

/******************************************************************************/
void MainTask::startCollecting(QString a_basepath, int a_nrjobs)
{
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::unittestFound,
                     this, &MainTask::onUnitTestFound);
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::collectionFinished,
                     this, &MainTask::onCollectionFinished);

    m_unitTestCollector->start(a_basepath, a_nrjobs);
}

/******************************************************************************/
void MainTask::onUnitTestFound(const QString &a_path)
{
    qCDebug(LogQtTestRunner, "%s", a_path.toStdString().c_str());
}

/******************************************************************************/
void MainTask::onCollectionFinished()
{
    qCDebug(LogQtTestRunner);
    emit finished();
}

/******************************************************************************/
