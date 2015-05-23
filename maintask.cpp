#include "maintask.h"
#include "logging.h"
#include "unittestfinder.h"


/******************************************************************************/
MainTask::MainTask(QObject *parent, const QString &a_basepath, UnitTestFinder *a_finder)
  : QObject(parent)
  , m_basepath(a_basepath)
  , m_finder(a_finder)
{
    qCDebug(LogQtTestRunner);
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


    QObject::connect(m_finder, &UnitTestFinder::finished,
                     this, &MainTask::finished);

    m_finder->start(m_basepath);
}

/******************************************************************************/
