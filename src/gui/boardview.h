/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2005 Michal Rudolf <mrudolf@kdewebdev.org>                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef __BOARDVIEW_H__
#define __BOARDVIEW_H__

#include "common.h"
#include "board.h"
#include "boardtheme.h"

#include <QWidget>

class BoardTheme;

/* the BoardView class represents a widget for displaying current
position on the screen. */

class BoardView : public QWidget
{
    Q_OBJECT
public:
    enum {WheelUp = Qt::LeftButton, WheelDown = Qt::RightButton};
    enum {IgnoreSideToMove = 1, AllowCopyPiece = 4};
    /* Create board widget. */
    BoardView(QWidget* parent, int flags = 0);
    /* Destroy widget. */
    ~BoardView();
    /* Set flags for board. Flags include:
    * @p IgnoreSideToMove - allow dragging all pieces (useful for setting up a position)
    */
    void setFlags(int flags);
	/* Update and shows current position. */
    void setBoard(const Board& value, int from = InvalidSquare, int to = InvalidSquare);
    /* @return displayed position. */
    Board board() const;
    /* @return current theme */
    const BoardTheme& theme() const;
    /* Flips/unflips board. */
    void setFlipped(bool flipped);
    /* @return @p true if board is displayed upside down. */
    bool isFlipped() const;
    /* Make it almost 1.555 : 1 */
    virtual int heightForWidth(int width) const;

    void showMoveIndicator(bool visible );

public slots:
    /* Reconfigure current theme. */
    void configure();

signals:
    /* User clicked source and destination squares */
    void moveMade(Square from, Square to, int button);
    /* User dragged and dropped a piece holding Control */
    void copyPiece(Square from, Square to);
    /* User dragged and dropped a piece holding Control */
    void invalidMove(Square from);
    /* User clicked square */
    void clicked(Square square, int button, QPoint pos);
    /* User moved mouse wheel. */
    void wheelScrolled(int dir);
    /* Indicate that a piece was dropped to the board */
    void pieceDropped(Square to, Piece p);
protected:
    /* Redraws whole board if necessary. */
    virtual void paintEvent(QPaintEvent*);
    /* Automatically resizes pieces and redisplays board. */
    virtual void resizeEvent(QResizeEvent*);
    /* Handle mouse events */
    virtual void mousePressEvent(QMouseEvent* e);
    /* Handle mouse events */
    virtual void mouseMoveEvent(QMouseEvent* e);
    /* Handle mouse events */
    virtual void mouseReleaseEvent(QMouseEvent* e);
    /* Handle mouse wheel events */
    virtual void wheelEvent(QWheelEvent* e);

protected: //Drag'n'Drop Support
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    /* Resizes pieces for new board size. */
    void resizeBoard();
    /* Calculate size and position of square */
    QRect squareRect(Square s);
    /* @return square at given position */
    Square squareAt(const QPoint& p) const;
    /* Selects given square. Previously selected square is unselected automatically. */
    void selectSquare(Square s);
    /* Unselects given square. */
    void unselectSquare();
    /* Check if piece at square @p square can be dragged */
    bool canDrag(Square s);
    /* Catch mouse events */
    bool eventFilter(QObject *obj, QEvent *ev);

    void drawSquares(QPaintEvent* event);
    void drawPieces(QPaintEvent* event);
    void drawMoveIndicator(QPaintEvent* event);
    void drawDraggedPieces(QPaintEvent* event);

    void drawSquareAnnotations(QPaintEvent* event);
    void drawSquareAnnotation(QPaintEvent* event, QString annotation);
    void drawArrowAnnotations(QPaintEvent* event);
    void drawArrowAnnotation(QPaintEvent* event, QString annotation);

    Board m_board;
    BoardTheme m_theme;
    bool m_flipped;
    bool m_showFrame;
    bool m_showCurrentMove;
    int m_selectedSquare;
    int m_hoverSquare;
    int m_hiFrom;
    int m_hiTo;
    int m_currentFrom;
    int m_currentTo;
    int m_flags;
	bool m_coordinates;
    int m_scale;
    Piece m_dragged;
    QPoint m_dragStart;
    QPoint m_dragPoint;
    int m_button;
    bool m_clickUsed;
    int m_wheelCurrentDelta;
    int m_minDeltaWheel;
    unsigned int m_moveListCurrent;
    bool m_showMoveIndicator;
};

class BoardViewMimeData : public QMimeData
{
    Q_OBJECT

public:
    Piece m_piece;
};

#endif

