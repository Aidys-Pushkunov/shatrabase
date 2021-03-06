/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   Copyright (C) 2012 by Jens Nissen jens-chessx@gmx.net                 *
 ***************************************************************************/

#include "logstream.h"
#include <QDebug>
#include <fstream>
#include <QTime>
#include <QDir>
//#define _DEBUG
using namespace std;
ofstream logfile;

static bool bDoLog = false;
void SimpleLoggingHandler(QtMsgType type, const char *msg)
{
    if (!bDoLog) return;

    switch (type)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5,5,0)
        case QtInfoMsg:
#endif
        case QtDebugMsg:
            logfile << QTime::currentTime().toString().toLatin1().data() << " : "         << msg << endl;
            break;
        case QtCriticalMsg:
            logfile << QTime::currentTime().toString().toLatin1().data() << " Critical: " << msg << endl;
            break;
        case QtWarningMsg:
            logfile << QTime::currentTime().toString().toLatin1().data() << " Warning: "  << msg << endl;
            break;
        case QtFatalMsg:
            logfile << QTime::currentTime().toString().toLatin1().data() <<  " Fatal: "   << msg << endl;
            break;
     }
}


void startFileLog()
{
#ifdef _DEBUG
/*
    QString currentPath = QDir::currentPath()+ "/shatrabase.log";
    qDebug() << currentPath;
    logfile.open(currentPath.toLatin1().data(),ios::app);
    bDoLog = true;
    qInstallMessageHandler(SimpleLoggingHandler);
*/
#endif
}
void stopFileLog()
{
    if (bDoLog)
    {
        bDoLog = false;
        logfile.close();
    }
}

