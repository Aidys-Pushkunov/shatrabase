/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *	 (C) 2006 Michal Rudolf <mrudolf@kdewebdev.org>                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __GAMELIST_H__
#define __GAMELIST_H__

#include "tableview.h"
#include "game.h"

class DatabaseInfo;
class DatabaseModel;
class SortFilterModel;

/** @ingroup GUI
The GameList class displays list of the games in current filter. It allows
user to click on list header and perform simple tag searches. */

class GameList : public TableView
{
	Q_OBJECT
public:
    GameList(DatabaseInfo * db, QWidget* parent = 0);
    ~GameList();

    /* Set current database */
public slots:
    /* Change current database */
    void setDatabaseInfo(DatabaseInfo* db);
    /* Update filter (called after changing filter outside) */
	void updateFilter();
    /* Perform simple search */
	void simpleSearch(int tag);
    /* Select and show current game in the list */
	void selectGame(int index);
    /* Select and show current game in the list */
    void slotFilterListByPlayer(QString ns);
    /* Select and show current event in the list */
    void slotFilterListByEvent(QString ns);
    /* Select and show current event and player in the list */
    void slotFilterListByEventPlayer(QString event, QString player);

private slots:
    /* Re-emit the request to the receivers to perform some action */
	void itemSelected(const QModelIndex& index);
    /* Request a context menu for the list members */
    void slotContextMenu(const QPoint& pos);
    /* Request a copy dialog for the game identified by m_cell */
    void slotCopyGame();
    /* Delete or undelete a game */
    void slotDeleteGame();

signals:
	void selected(int);
	void searchDone();
    void raiseRequest();
    void requestCopyGame();
    void requestDeleteGame();
    void requestGameData(Game&);

protected: //Drag'n'Drop Support
    void startToDrag(const QModelIndex&);

private:
    DatabaseInfo * m_db;
    DatabaseModel * m_model;
    SortFilterModel * m_sort;
};

#endif
