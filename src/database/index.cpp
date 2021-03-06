/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2005-2006 Marius Roets <roets.marius@gmail.com>                   *
 *   (C) 2007 Rico Zenklusen <rico_z@users.sourceforge.net>                *
 *   (C) 2007-2009 Michal Rudolf <mrudolf@kdewebdev.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QtDebug>
#include <QFile>
#include <QDataStream>
#include <QHash>
#include <QMultiHash>

#include "index.h"

Index::Index()
{
    // Dummy Values in case a index is miscalculated
    m_tagNames[0]  = "?";
    m_tagValues[0] = "?";
}

Index::~Index()
{
}

SHATRA::GameId Index::add()
{
    int gameId = m_indexItems.count();
    m_indexItems.append(new IndexItem);
    m_deletedGames.append(false);
    m_validFlags.append(true);
    return gameId;
}

SHATRA::TagIndex Index::AddTagName(QString name)
{
    if (m_tagNameIndex.contains(name))
    {
        return m_tagNameIndex.value(name);
    }
    int n = m_tagNameIndex.size();
    m_tagNameIndex[name] = n;
    m_tagNames[n] = name;
    return n;
}

SHATRA::ValueIndex Index::AddTagValue(QString name)
{
    if (m_tagValueIndex.contains(name))
    {
        return m_tagValueIndex[name];
    }
    int n = m_tagValueIndex.size();
    m_tagValueIndex[name] = n;
    m_tagValues[n] = name;
    return n;
}

void Index::setTag(const QString& tagName, const QString& value, int gameId)
{
    SHATRA::TagIndex tagIndex = AddTagName(tagName);
    SHATRA::ValueIndex valueIndex = AddTagValue(value);

    if (m_indexItems.count() <= gameId)
    {
        add();
    }
    m_indexItems[gameId]->set(tagIndex, valueIndex);
    m_mapTagToIndexItems.insertMulti(tagIndex, gameId);
}

void Index::setValidFlag(const int& gameId, bool value)
{
    m_validFlags[gameId]=value;
}

bool Index::isValidFlag(const int& gameId) const
{
    return m_validFlags[gameId];
}

bool Index::write(QDataStream &out) const
{
    out << m_tagNames;
    out << m_tagValues;
    out << m_indexItems.count();
    for (int i=0; i<m_indexItems.count();++i)
    {
        m_indexItems[i]->write(out);
    }
    out << m_validFlags;

    bool extension = false;
    out << extension;

    return true;
}

bool Index::read(QDataStream &in, volatile bool *breakFlag)
{
    in >> m_tagNames;
    in >> m_tagValues;
    int itemCount;
    in >> itemCount;

    int countDiff = itemCount/100;
    int nextDiff = countDiff;
    int percentDone = 0;

    for (int i=0; i<itemCount;++i)
    {
        if (*breakFlag) return false;
        add();
        m_indexItems[i]->read(in);
        if (i >= nextDiff)
        {
           nextDiff += countDiff;
           emit progress(++percentDone);
        }
    }
    in >> m_validFlags;

    bool extension;
    in >> extension;

    calculateReverseMaps(breakFlag);
    calculateTagMap(breakFlag);

    return !(*breakFlag);
}

void Index::clearCache()
{
    m_tagNameIndex.clear();
    m_tagValueIndex.clear();
    m_mapTagToIndexItems.clear();
}

void Index::calculateCache(volatile bool* breakFlag)
{
    calculateReverseMaps(breakFlag);
    calculateTagMap(breakFlag);
}

void Index::calculateReverseMaps(volatile bool* breakFlag)
{
    if (m_tagNameIndex.isEmpty())
    {
        foreach (SHATRA::TagIndex tagIndex, m_tagNames.keys())
        {
            if (breakFlag && *breakFlag) return;
            m_tagNameIndex.insert(m_tagNames.value(tagIndex), tagIndex);
        }
    }
    if (m_tagValueIndex.isEmpty())
    {
        foreach (SHATRA::ValueIndex valueIndex, m_tagValues.keys())
        {
            if (breakFlag && *breakFlag) return;
            m_tagValueIndex.insert(m_tagValues.value(valueIndex), valueIndex);
        }
    }
}

void Index::calculateTagMap(volatile bool *breakFlag)
{
    if (m_mapTagToIndexItems.isEmpty())
    {
        foreach (SHATRA::TagIndex tagIndex, m_tagNames.keys())
        {
            for (SHATRA::GameId gameId=0; gameId<(SHATRA::GameId)m_indexItems.size(); ++gameId)
            {
                if (breakFlag && *breakFlag)
                    return;
                if (indexItemHasTag(tagIndex, gameId))
                {
                    m_mapTagToIndexItems.insertMulti(tagIndex, gameId);
                }
            }
        }
    }
}

void Index::clear()
{
    for (int i = 0 ; i < m_indexItems.count() ; ++i)
    {
        delete m_indexItems[i];
	}
	m_indexItems.clear();
    m_tagNames.clear();
    m_tagNameIndex.clear();
    m_tagValues.clear();
    m_tagValueIndex.clear();
    m_deletedGames.clear();
    m_validFlags.clear();
    m_mapTagToIndexItems.clear();
}

int Index::count() const
{
    return m_indexItems.count();
}

QBitArray Index::listContainingValue(const QString& tagName, const QString& value) const
{
    SHATRA::TagIndex tagIndex = m_tagNameIndex.value(tagName);
    SHATRA::ValueIndex valueIndex = m_tagValueIndex.value(value);

    QBitArray list(count(), false);
    for (int i = 0; i < count(); ++i) {
        list.setBit(i, m_indexItems[i]->valueIndex(tagIndex)==valueIndex);
    }
    return list;
}

QBitArray Index::listInRange(const QString& tagName,
                             const QString& minValue, const QString& maxValue) const
{
    SHATRA::TagIndex tagIndex = m_tagNameIndex.value(tagName);

    QBitArray list(count(), false);
    for (int i = 0; i < count(); ++i) {
        QString value = tagValue(tagIndex,i);
        list.setBit(i, (minValue < value) && (value < maxValue));
    }
    return list;
}

QBitArray Index::listPartialValue(const QString& tagName, const QString& value) const
{
    SHATRA::TagIndex tagIndex = m_tagNameIndex.value(tagName);

    QBitArray list(count(), false);
    for (int i = 0; i < count(); ++i) {
        QString gameValue = tagValue(tagIndex,i);
        list.setBit(i, gameValue.contains(value, Qt::CaseInsensitive));
    }
    return list;
}

QString Index::tagValue(SHATRA::TagIndex tagIndex, int gameId) const
{
    SHATRA::ValueIndex valueIndex = m_indexItems[gameId]->valueIndex(tagIndex);

    return m_tagValues.value(valueIndex);
}

QString Index::tagName(SHATRA::TagIndex tagIndex) const
{
    return m_tagNames.value(tagIndex);
}

QString Index::tagValueName(SHATRA::ValueIndex valueIndex) const
{
    return m_tagValues.value(valueIndex);
}

QString Index::tagValue(const QString& tagName, SHATRA::GameId gameId) const
{
    SHATRA::TagIndex tagIndex = m_tagNameIndex.value(tagName);
    return tagValue(tagIndex, gameId);
}

SHATRA::ValueIndex Index::valueIndexFromTag(
        const QString& tagName, SHATRA::GameId gameId) const
{
    SHATRA::TagIndex tagIndex = m_tagNameIndex.value(tagName);
    return m_indexItems[gameId]->valueIndex(tagIndex);
}

bool Index::indexItemHasTag(SHATRA::TagIndex tagIndex, SHATRA::GameId gameId) const
{
    return m_indexItems[gameId]->hasTagIndex(tagIndex);
}

inline SHATRA::ValueIndex Index::valueIndexFromIndex(
        SHATRA::TagIndex tagIndex, SHATRA::GameId gameId) const
{
    return m_indexItems[gameId]->valueIndex(tagIndex);
}

SHATRA::TagIndex Index::getTagIndex(const QString& value) const
{
    return m_tagNameIndex.value(value);
}

SHATRA::ValueIndex Index::getValueIndex(const QString& value) const
{
    return m_tagValueIndex.value(value);
}

IndexItem* Index::item(int gameId)
{
	return m_indexItems[gameId];
}

void Index::loadGameHeaders(SHATRA::GameId id, Game& game)
{
    game.clearTags();
    foreach (SHATRA::TagIndex tagIndex, m_indexItems[id]->getTagMapping().keys())
    {
        // qDebug() << "lGH>" << &game << " " << id << " " << tagName(tagIndex) << " " << tagValue(tagIndex, id);
        game.setTag(tagName(tagIndex), tagValue(tagIndex, id));
    }
}

QStringList Index::playerNames() const
{
    QStringList allPlayerNames;
    QSet<SHATRA::ValueIndex> playerNameIndex;

    SHATRA::TagIndex tagIndex = getTagIndex(SHATRA::TagNameWhite);

    foreach(int gameId, m_mapTagToIndexItems.values(tagIndex))
    {
        playerNameIndex.insert(m_indexItems[gameId]->valueIndex(tagIndex));
    }

    tagIndex = getTagIndex(SHATRA::TagNameBlack);

    foreach (int gameId, m_mapTagToIndexItems.values(tagIndex))
    {
        playerNameIndex.insert(m_indexItems[gameId]->valueIndex(tagIndex));
    }

    foreach (SHATRA::ValueIndex valueIndex, playerNameIndex)
    {
        allPlayerNames.append(tagValueName(valueIndex));
    }

    return allPlayerNames;
}

QStringList Index::tagValues(const QString& tagName) const
{
    QStringList allTagNames;
    QSet<SHATRA::ValueIndex> tagNameIndex;
    SHATRA::TagIndex tagIndex = getTagIndex(tagName);

    foreach(int gameId, m_mapTagToIndexItems.values(tagIndex))
    {
        tagNameIndex.insert(m_indexItems[gameId]->valueIndex(tagIndex));
    }

    foreach(SHATRA::ValueIndex valueIndex, tagNameIndex)
    {
        allTagNames.append(tagValueName(valueIndex));
    }

    return allTagNames;
}

bool Index::deleted(const int& gameId) const
{
    return m_deletedGames[gameId];
}

void Index::setDeleted(int gameId, bool df)
{
    m_deletedGames[gameId]=df;
}
