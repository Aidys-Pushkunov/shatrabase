/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2010 Michal Rudolf <mrudolf@kdewebdev.org>                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "enginelist.h"
#include "settings.h"

bool EngineList::m_wasEmpty = false;

EngineList::EngineList()
{
}

void EngineList::restoreEmptyFromPath(QString path, EngineData::EngineProtocol protocol)
{
    qDebug() << "enginelist: " << path;
    m_wasEmpty = true;

    if (path.isEmpty()) return;

    QStringList engines = QDir(path).entryList(QDir::Executable | QDir::Files | QDir::NoSymLinks);

    for (int i = 0; i < engines.size(); ++i)
    {
        QString key = QString::number(i);
        QString name = engines[i];
        EngineData data(name);
        data.command = path + "/" + name;
        data.directory = path;
        data.protocol = protocol;
        append(data);
        qDebug() << "automatically added Engine " << name;
    }
}

void EngineList::restoreEmpty()
{
    m_wasEmpty = true;
    restoreEmptyFromPath(AppSettings->enginePath(), EngineData::USHI);
/*
    QString path(AppSettings->dataPath());

#ifdef Q_OS_WIN
    QString path1 = path + "/engines/ushi";
//    QString path2 = path + "/engines/winboard";
#endif

#ifdef Q_OS_MAC
    QString path1 = path + "/engines-mac/ushi";
//    QString path2 = path + "/engines-mac/winboard";
#endif

#ifdef Q_OS_LINUX
    QString path1 = path + "/engines-linux/ushi";
//    QString path2 = path + "/engines-linux/winboard";
#endif

    restoreEmptyFromPath(path1, EngineData::USHI);
//    restoreEmptyFromPath(path2, EngineData::WinBoard);
*/
}

void EngineList::restore()
{
    clear();
    AppSettings->beginGroup("/Engines/");
    QStringList engines = AppSettings->childGroups();
    bool modified = false;
    if (engines.size())
    {
        for (int i = 0; i < engines.size(); ++i)
        {
            QString key = QString::number(i);
            QString name = AppSettings->value(key + "/Name").toString();
            EngineData data(name);
            data.command = AppSettings->value(key + "/Command").toString();
            data.options = AppSettings->value(key + "/Options").toString();
            data.directory = AppSettings->value(key + "/Directory").toString();
            QString protocolName = AppSettings->value(key + "/Protocol").toString();
//            if (protocolName == "WinBoard")
//                data.protocol = EngineData::WinBoard;
//            else 
            data.protocol = EngineData::USHI;
            AppSettings->getMap(key + "/OptionValues", data.m_optionValues);
            append(data);
        }
    }
    else
    {
        restoreEmpty();
        modified = true;
    }
    AppSettings->endGroup();
    if (modified) save();
}

void EngineList::save()
{
    AppSettings->beginGroup("/Engines/");
    AppSettings->remove("");    // Clear group
    for (int i = 0; i < count(); ++i) {
        QString key = QString::number(i);
        AppSettings->setValue(key + "/Name", at(i).name);
        AppSettings->setValue(key + "/Command", at(i).command);
        if (!at(i).options.isEmpty())
            AppSettings->setValue(key + "/Options", at(i).options);
        if (!at(i).directory.isEmpty())
            AppSettings->setValue(key + "/Directory", at(i).directory);
//      if (at(i).protocol == EngineData::WinBoard)
//          AppSettings->setValue(key + "/Protocol", "WinBoard");
//      else
        AppSettings->setValue(key + "/Protocol", "USHI");
        AppSettings->setMap(key+"/OptionValues", at(i).m_optionValues);
    }
    AppSettings->endGroup();
}

QStringList EngineList::names() const
{
    QStringList engineNames;
    for (int i = 0; i < count(); ++i)
        engineNames.append(at(i).name);
    return engineNames;
}
