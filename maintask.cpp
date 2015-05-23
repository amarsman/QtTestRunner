#include "maintask.h"

/******************************************************************************/
MainTask::MainTask(QObject *parent) : QObject(parent), category("MainTask")
{
    qCDebug(category);
}

/******************************************************************************/
MainTask::~MainTask()
{
    qCDebug(category);
}

/******************************************************************************/
void MainTask::run()
{
    qCDebug(category);

    emit finished();
}

/******************************************************************************/
