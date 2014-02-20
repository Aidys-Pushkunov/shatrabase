 /**************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2005 William Hoggarth <whoggarth@users.sourceforge.net>           *
 *   (C) 2005-2009 Michal Rudolf <mrudolf@kdewebdev.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef __BOARDLIST_H__
#define __BOARDLIST_H__

#include "move.h"
#include "urvct.hpp"

class BoardList : public urvct<Move,256>
{
  public:
    BoardList() : urvct<Move,256>() {}
    inline Move& add() { append(Move()); return back(); }    
};

#endif // __BOARDLIST_H__
