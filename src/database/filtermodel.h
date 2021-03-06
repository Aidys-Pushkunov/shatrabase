/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2007 Marius Roets <roets.marius@gmail.com>                        *
 *   (C) 2007-2009 Michal Rudolf <mrudolf@kdewebdev.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef __FILTERMODEL_H__
#define __FILTERMODEL_H__

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QStringList>
#include <QHash>
#include <QVariant>

#include "filter.h"
#include "game.h"


/*
  The FilterModel class is an interface to Database used with Qt
  Model/View architecture
*/
class FilterModel: public QAbstractItemModel
{
	Q_OBJECT

    /** bitshift for cache value.
     *  Implicitly defines max number of columns!! */
    static const int MAX_COLUMNS_SHIFT = 8;

public:
    /** Constructs a FilterModel object using a pointer to a Filter */
	FilterModel(Filter* filter, QObject *parent = 0);
	~FilterModel();

    /** Returns the number of rows in the model */
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    /** Returns the number of columns in the model */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    /** Returns an item of data given the item 'index' */
	virtual QVariant data(const QModelIndex &index, int role) const;
    /** Returns the header information for header 'section' */
	virtual QVariant headerData(int section, Qt::Orientation orientation,
				    int role = Qt::DisplayRole) const;
    /** No tree - always return invalid parent */
	virtual QModelIndex parent(const QModelIndex&) const  {return QModelIndex();}
    /** No tree - always return self */
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    /** No children */
    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const { return !parent.isValid(); }
    /** Associated filter */
	virtual Filter* filter();
    /** Changes current database. Resets any views. */
	virtual void setFilter(Filter* filter);

    //virtual void sort(int column, Qt::SortOrder order);

    /** Get the column tags. */
    const QStringList GetColumnTags()  { return m_columnTags; }
    /** Get the column names. */
    const QStringList GetColumnNames() { return m_columnNames; }

private:
    /** A pointer to filter on which the model operates */
	Filter* m_filter;
    /** The column names of the model */
	QStringList m_columnNames;
    /** Map of columns and database tags */
	QStringList m_columnTags;
    /** A pointer to a game object, to hold the retrieved information
	 * about the game */
	Game* m_game;
    /* Current game index - used for caching */
    //mutable int m_gameIndex;
    /** cache for game values */
    typedef quint64 Hash;
    mutable QHash<Hash, QVariant> m_cache;
    typedef QHash<Hash, QVariant>::const_iterator CacheIter;
};

#endif	// __FilterModelBase_H__

