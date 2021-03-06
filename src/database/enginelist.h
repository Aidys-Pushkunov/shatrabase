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

#ifndef ENGINELIST_H
#define ENGINELIST_H

#include <QtCore>
#include "enginedata.h"


/** The EngineList class contains configuration of all installed engines. */
class EngineList : public QList<EngineData>
{
public:
	/** Standard constructor. */
	EngineList();

	/** Restore configuration from INI file. */
	void restore();
	/** Store configuration in INI file. */
	void save();
	/** Retrieve list of engine names in current order. */
    QStringList names() const;

    /** True when there was no Engine in list on start of the program */
    bool wasEmpty() const { return m_wasEmpty; }

protected:
    /** Set the content of the default engine directories into the list of engines */
    void restoreEmpty();
    /** Set the content of the given directory into the list of engines */
    void restoreEmptyFromPath(QString path, EngineData::EngineProtocol protocol);

    static bool m_wasEmpty;
};

#endif // ENGINELIST_H
