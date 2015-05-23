#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QThreadPool>
#include "unittestfinder.h"
#include "logging.h"

/******************************************************************************/
UnitTestFinder::UnitTestFinder()
    : m_TextPattern("")
    , m_SearchPath("")
    , m_FilePattern("")
    , m_StopRequested(false)
    , m_Running(false)
{
    qCDebug(LogQtTestRunner);
    setAutoDelete(false);
}

/******************************************************************************/
UnitTestFinder::~UnitTestFinder()
{
    qCDebug(LogQtTestRunner);
}

/******************************************************************************/
void UnitTestFinder::start(const QString &a_TextPattern,
                           const QString &a_SearchPath,
                           const QString &a_FilePattern)
{
    qCDebug(LogQtTestRunner);
    if (m_Running) return;

    m_TextPattern = a_TextPattern;
    m_SearchPath = a_SearchPath;
    m_FilePattern = a_FilePattern;
    m_StopRequested = false;

    QThreadPool::globalInstance()->start(this);
}

/******************************************************************************/
void UnitTestFinder::stop()
{
    qCDebug(LogQtTestRunner);
    if (!m_Running) return;

    qDebug("FileFinder Stopping...");
    m_StopRequested = true;
}

/******************************************************************************/
void UnitTestFinder::run()
{
    qCDebug(LogQtTestRunner);
    if (m_Running)
    {
        return;
    }

    m_Running = true;

    QDirIterator it(m_SearchPath,
                    QStringList() << m_FilePattern,
                    QDir::Files,
                    QDirIterator::Subdirectories);

    while (!m_StopRequested && it.hasNext())
    {
        it.next();
        QDir dir(it.filePath());
        QString fullPath = dir.absolutePath();

        QFile data(fullPath);
        if (data.open(QFile::ReadOnly))
        {
            QTextStream instr(&data);
            QStringList stringList;

            // Read all lines
            while (!m_StopRequested)
            {
                QString line = instr.readLine();
                if (line.isNull()) break;
                stringList.append(line);
            }

            // Find text in all lines
            QStringList::iterator it;
            for (it = stringList.begin(); !m_StopRequested && it != stringList.end(); ++it)
            {
                QString &line = *it;

                if (line.contains(m_TextPattern))
                {
                    //FileFound filefound;
                    //filefound.m_Filename = fullPath;
                    //m_fileFoundModel->addFileFound(filefound);

                    //emit resultFound(fullPath);
                    //qDebug("%s", line.toLatin1().data());
                }
            }
        }
    }

    m_Running = false;
    qCDebug(LogQtTestRunner, "Finished");
    emit finished();
}

/******************************************************************************/
