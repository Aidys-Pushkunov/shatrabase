/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2005 Ejner Borgbjerg <ejner@users.sourceforge.net>                *
 *   (C) 2009 Michal Rudolf <mrudolf@kdewebdev.org>                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/**
DatabaseConversion contains utility functions to convert from various formats
into ChessX format.
*/

#ifndef __DATABASECONVERSION_H__
#define __DATABASECONVERSION_H__

#include "playerdatabase.h"
#include <QDir>

class DatabaseConversion : public QObject
{

public:
	/**
	convert Scid ratings.ssp to player database
	outFileName is filename without extension
	inFileName is full path for ratings.ssp
	pictureDir is directory where player pictures are -
	format can be jpg, gif, or any format that QImage(file) will
	understand
	*/
	static bool playerDatabaseFromScidRatings(const QString& inFileName, const QString& outFileName, const QString& pictureDir);

};

#endif
