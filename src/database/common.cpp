/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2005 William Hoggarth <whoggarth@users.sourceforge.net>           * 
 *   (C) 2006-2009 Michal Rudolf <mrudolf@kdewebdev.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "common.h"
/*
Piece& operator++(Piece& w)
{
    if (w<BlackShatra)
        return w = Piece(w+1);
    else
        return w = Empty;
}

Piece operator++(Piece& w, int)
{
    Piece alt = w;
    ++w;
    return alt;
}
*/
bool g_loading          = false;
bool g_searching        = false;
bool g_notation         = NUM;  // default is numeric
bool g_nChanged         = false;
bool g_autoResultOnLoad = false;
bool g_resModified;
int  g_nErrors          = 0;
int  g_aveNodes         = 0;
int  g_totalNodes       = 0;
int  g_version          = 2;    // for now, default is 'chess version'

QString startPosition()
{
    switch (g_version ) {
    case 1 :
        return "SSSSSSSSS/K/SSSSSSS/SSSSSSS/7/7/sssssss/sssssss/k/sssssssss w Tt - - - 1";
    case 2 :
        return "SQSSRSBRB/K/SSSSSSS/7/7/7/7/sssssss/k/brbsrssqs w Tt - - - 1";
    }
    return "";
}

QString resultString(Result result)
{
	switch (result) {
    case ResultUnknown:
		return "*";
	case WhiteWin:
		return "1-0";
	case Draw:
		return "1/2-1/2";
	case BlackWin:
		return "0-1";
	default:
		return "*";
	}
}

