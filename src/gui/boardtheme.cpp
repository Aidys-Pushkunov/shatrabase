/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2005 Michal Rudolf <mrudolf@kdewebdev.org>                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "boardtheme.h"
#include "settings.h"
#include "messagedialog.h"

#include <QDebug>
#include <QPixmap>

BoardTheme::BoardTheme()
{
    m_pieceFilename = "new QString()";
    m_originalUrgent.load(":/images/urgent.png");
    m_originalTowerOverlay.load(":/images/square_emboss.png");
    m_originalEpOverlay.load(":/images/square_emboss_sunken.png");
}

BoardTheme::~BoardTheme()
{}

const QPixmap& BoardTheme::piece(SHATRA::Piece p, bool flipped) const
{
    if (flipped)
    {
        if (p == SHATRA::WhiteBatyr)
            return m_piece[SHATRA::ConstPieceTypes];
        if (p == SHATRA::BlackBatyr)
            return m_piece[SHATRA::ConstPieceTypes+1];
    }
    return m_piece[p];
}

const QPixmap& BoardTheme::originalPiece(SHATRA::Piece p, bool flipped) const
{
    if (flipped)
    {
        if (p == SHATRA::WhiteBatyr)
            return m_originalPiece[SHATRA::ConstPieceTypes];
        if (p == SHATRA::BlackBatyr)
            return m_originalPiece[SHATRA::ConstPieceTypes+1];
    }
    return m_originalPiece[p];
}

const QPixmap& BoardTheme::square(bool dark) const
{
    return m_square[dark?1:0];
}

const QPixmap& BoardTheme::urgent() const
{
    return m_urgent;
}

const QPixmap& BoardTheme::towerOverlay() const
{
    return m_towerOverlay;
}

const QPixmap& BoardTheme::enPassantOverlay() const
{
    return m_epOverlay;
}


const QPixmap& BoardTheme::originalSquare(bool dark) const
{
    return m_originalSquare[dark?1:0];
}

QString BoardTheme::pieceThemeName() const
{
	int start = m_pieceFilename.lastIndexOf('/') + 1;
	return m_pieceFilename.mid(start + 1, m_pieceFilename.length() - start - 4);
}

QString BoardTheme::boardThemeName() const
{
	int start = m_boardFilename.lastIndexOf('/') + 1;
	return m_boardFilename.mid(start + 1, m_pieceFilename.length() - start - 4);
}

bool BoardTheme::isValid() const
{
	return !m_pieceFilename.isNull();
}

bool BoardTheme::loadPieces(const QString& pieces, int effect)
{
    QString effectPath;
    if (effect == Shadow) effectPath = "shadow";
    QString themePath = QString("%1/%2/%3.png").arg(themeDirectory()).arg(effectPath).arg(pieces);

	QPixmap big;
	if (!big.load(themePath) || big.width() < 128)
		return false;

    int realsize = big.height() / 3;
    bool hasFlippedBatyr = (realsize == big.width() / 6);
    if (realsize != big.width() / 5 && !hasFlippedBatyr)
		return false;
    int rs2 = realsize + realsize;

	/* Cut big theme bitmap into separate pieces */
    m_originalPiece[SHATRA::WhiteTura] =   big.copy(0 * realsize, 0, realsize, realsize);
    m_originalPiece[SHATRA::WhiteYalkyn] = big.copy(1 * realsize, 0, realsize, realsize);
    m_originalPiece[SHATRA::WhiteBatyr] =  big.copy(2 * realsize, 0, realsize, realsize);
    m_originalPiece[SHATRA::WhiteBiy] =    big.copy(3 * realsize, 0, realsize, realsize);
    m_originalPiece[SHATRA::WhiteShatra] = big.copy(4 * realsize, 0, realsize, realsize);
    m_originalPiece[SHATRA::BlackTura] =   big.copy(0 * realsize, realsize, realsize, realsize);
    m_originalPiece[SHATRA::BlackYalkyn] = big.copy(1 * realsize, realsize, realsize, realsize);
    m_originalPiece[SHATRA::BlackBatyr] =  big.copy(2 * realsize, realsize, realsize, realsize);
    m_originalPiece[SHATRA::BlackBiy] =    big.copy(3 * realsize, realsize, realsize, realsize);
    m_originalPiece[SHATRA::BlackShatra] = big.copy(4 * realsize, realsize, realsize, realsize);
    m_originalPiece[SHATRA::WasTura] =     big.copy(0 * realsize, rs2, realsize, realsize);
    m_originalPiece[SHATRA::WasYalkyn] =   big.copy(1 * realsize, rs2, realsize, realsize);
    m_originalPiece[SHATRA::WasBatyr] =    big.copy(2 * realsize, rs2, realsize, realsize);
    m_originalPiece[SHATRA::WasShatra] =   big.copy(4 * realsize, rs2, realsize, realsize);
    if (hasFlippedBatyr)
    {
        m_originalPiece[SHATRA::ConstPieceTypes] =
                                   big.copy(5 * realsize, 0, realsize, realsize);
        m_originalPiece[SHATRA::ConstPieceTypes+1] =
                                   big.copy(5 * realsize, realsize, realsize, realsize);
    }
    else
    {
        m_originalPiece[SHATRA::ConstPieceTypes] = m_originalPiece[SHATRA::WhiteBatyr];
        m_originalPiece[SHATRA::ConstPieceTypes+1] = m_originalPiece[SHATRA::BlackBatyr];
    }

    m_pieceFilename = themePath;

	if (size().isEmpty())
		setSize(QSize(realsize, realsize));
    else setSize(size());
	return true;
}

bool BoardTheme::loadBoard(const QString& board)
{
    if (board.isEmpty())
    {
		m_boardFilename = board;
		updateSquares();
		return true;
	}
    QString themePath = QString("%1/%2.png").arg(boardDirectory()).arg(board);
	QPixmap big;
	if (!big.load(themePath))
		return false;
	int realsize = big.height() / 2;
	if (realsize != big.width())
		return false;

	m_originalSquare[0] = big.copy(0, 0, realsize, realsize);
	m_originalSquare[1] = big.copy(0, realsize, realsize, realsize);
	m_boardFilename = themePath;

	updateSquares();
	return true;
}

void BoardTheme::configure()
{
    AppSettings->beginGroup("/Board/");
    setColor(BoardTheme::LightSquare, AppSettings->getValue("lightColor").value<QColor>());
    setColor(BoardTheme::DarkSquare, AppSettings->getValue("darkColor").value<QColor>());
    setColor(BoardTheme::Highlight, AppSettings->getValue("highlightColor").value<QColor>());
    setColor(BoardTheme::Frame, AppSettings->getValue("frameColor").value<QColor>());
    setColor(BoardTheme::CurrentMove, AppSettings->getValue("currentMoveColor").value<QColor>());
    setColor(BoardTheme::Background1, AppSettings->getValue("backgroundColor").value<QColor>());
    setColor(BoardTheme::Background2, AppSettings->getValue("backgroundColor2").value<QColor>());
    QString pieceTheme = AppSettings->getValue("pieceTheme").toString();
    int pieceEffect = AppSettings->getValue("pieceEffect").toInt();
    QString boardTheme = AppSettings->getValue("boardTheme").toString();
    AppSettings->endGroup();

    if (!loadPieces(pieceTheme, pieceEffect) &&
         !loadPieces(pieceTheme, Plain) &&
            !loadPieces("motifshatra", Plain))
    {
        MessageDialog::error(tr("Cannot find piece data.\nPlease check your installation."));
    }
    loadBoard(boardTheme);
}

QSize BoardTheme::size() const
{
	return m_size;
}

QPoint BoardTheme::pieceCenter() const
{
	return QPoint(m_size.width() / 2, m_size.height() / 2);
}

QRect BoardTheme::rect() const
{
	return QRect(QPoint(0, 0), m_size);
}

void BoardTheme::setSize(const QSize& value)
{
	if (!isValid())
		return;
	m_size = value;
    for (int i = 1; i < SHATRA::ConstPieceTypes+2; i++)
        if (i != SHATRA::InvalidPiece)
            m_piece[i] = m_originalPiece[i].scaled(m_size,
                Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    updateSquares();
    m_urgent = m_originalUrgent.scaled(m_size,
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_towerOverlay = m_originalTowerOverlay.scaled(m_size,
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_epOverlay = m_originalEpOverlay.scaled(m_size,
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

void BoardTheme::updateSquares()
{
	if (!isValid())
		return;
    bool scale = m_size.width() > m_originalPiece[SHATRA::WhiteTura].width()
              || m_size.height() > m_originalPiece[SHATRA::WhiteTura].height()
			  || m_size.width() < 30 || m_size.height() < 30;
	if (isBoardPlain()) {
		m_square[0] = QPixmap(m_size);
		m_square[0].fill(color(LightSquare).rgb());
		m_square[1] = QPixmap(m_size);
		m_square[1].fill(color(DarkSquare).rgb());
	} else if (scale) {
		m_square[0] =  m_originalSquare[0].scaled(size(), Qt::IgnoreAspectRatio,
				Qt::SmoothTransformation);
		m_square[1] =  m_originalSquare[1].scaled(size(), Qt::IgnoreAspectRatio,
				Qt::SmoothTransformation);
	} else {
        m_square[0] =  m_originalSquare[0].copy(rect());
        m_square[1] =  m_originalSquare[1].copy(rect());
	}
}

QString BoardTheme::themeDirectory() const
{
    QString path = AppSettings->dataPath() + "/themes";
    if (QFile::exists(path))
        return path;
    else
        return QString(":/themes");
}

QString BoardTheme::boardDirectory() const
{
    QString path = AppSettings->dataPath() + "/themes/boards";
    if (QFile::exists(path))
        return path;
    else
        return QString(":/themes/boards");
}

bool BoardTheme::isBoardPlain() const
{
	return m_boardFilename.isEmpty();
}

void BoardTheme::setColor(ColorRole role, const QColor& value)
{
	m_colors[role] = value;
	if (role == LightSquare || role == DarkSquare)
		updateSquares();
}

QColor BoardTheme::color(ColorRole role) const
{
	return m_colors[role];
}

