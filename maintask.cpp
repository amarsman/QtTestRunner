#include <QProcess>
#include <QString>
#include <QRegularExpression>

#include "maintask.h"
#include "logging.h"
#include "unittestcollector.h"


/******************************************************************************/
MainTask::MainTask(QObject *parent, const QString &a_basepath)
    : QObject(parent)
    , m_basepath(a_basepath)
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

    startCollecting();
}

/******************************************************************************/
void MainTask::startCollecting()
{
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::unittestFound,
                     this, &MainTask::onUnitTestFound);
    QObject::connect(m_unitTestCollector.data(), &UnitTestCollector::collectionFinished,
                     this, &MainTask::onCollectionFinished);

    m_unitTestCollector->start(m_basepath);
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
