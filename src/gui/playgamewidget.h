/***************************************************************************

Copyright (C) 2014  stefan.berke @ modular-audio-graphics.com

This source is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

****************************************************************************/

#ifndef PLAYERSELECT_H
#define PLAYERSELECT_H

#include <QWidget>
#include <QList>

#include "board.h"
#include "engine.h"
#include "playtimecontrol.h"
#include "qled.h"

namespace SHATRA { class Board; }
class PlayGame;
class EngineDebugWidget;

namespace Ui { class PlayGame; }

/** More than a GUI wrapper around PlayGame class.
 *
 *  Actually, this class handles all the gui integration of gaming
 *  and the time control!
 */
class PlayGameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayGameWidget(EngineDebugWidget * debug, QWidget *parent = 0);
    ~PlayGameWidget();

    /** Is there a tournament mode set-up? */
    bool isTournament() const;

    /** Is any of the players a human? */
    bool isHumanInvolved() const;

    /** Returns whether the given side to move is controlled by the user. */
    bool isHuman(SHATRA::Color stm) const;

    /** show info lines? */
    bool doInfoLines() const;

    /** should MainWindow save the current game and start a new one? */
    bool doAutoSaveAndContinue() const;

    bool isPlaying() const { return playing_; }

    /** Should the board be updated?
        False only possible in engine vs. engine */
    bool isBoardUpdate() const;

    /** Returns if White should be able to interact with board */
    bool whiteCanMove() const;
    /** Returns if Black should be able to interact with board */
    bool blackCanMove() const;

    /** Return a spn tag style result.
        Empty when unknown. */
    QString resultString() const;

signals:
    /** Please open the game properties */
    void configureGameRequest();

    /** Emitted when a new game wants to be played */
    void startNewGameRequest(const QMap<QString, QString>& tags);

    /** Emitted when the current game in Chessbrowser should be continued. */
    void continueGameRequest();

    /** Emitted when player needs a break. */
    void pauseGame();

    /** The Engine has send a new position. */
    void moveMade(SHATRA::Move);

    /** This class wants to add a comment at current position */
    void gameComment(const QString&);

    void playerWins();
    void playerLoses();
    /** On engine vs. engine result */
    void gameEnded();

public slots:

    /** Applies app settings */
    void slotReconfigure();

    /** Tell PlayGameWidget to 'click new game' */
    void startNewGame();

    /** Send this as an answer to setNewGameRequest(). */
    void startNewGameOk();

    /** Stops play mode. E.g. when save current game was cancelled. */
    void stop();

    /** Stops any connected Engines and discards their next moves.
        XXX Not possible to restart thinking right now. */
    void stopThinking();

    /** Sets new position. Signals that a move has been performed.
        If required, the Engine will be queried. */
    void setPosition(const SHATRA::Board& board);

    /** Continue a game from the current possiton */
    void continuePosition(const SHATRA::Board& board);

    /** Signal that board has done the last issued move */
    void animationFinished(const SHATRA::Board& board);

private slots:

    // -- engine callbacks --

    void enginesReady();
    void engineClueless();
    void engineError(const QString&);
    void moveFromEngine(SHATRA::Move);
    void infoFromEngine(SHATRA::Move, int);

    // -- widget callbacks --

    void slotName1Changed_(const QString&);
    void slotName2Changed_(const QString&);

    void slotEngine1Changed_(const QString&);
    void slotEngine2Changed_(const QString&);

    void slotConfig1Clicked_();
    void slotConfig2Clicked_();

    void slotBlinkTimer_();
    void slotUpdateClocks_();
    void slotTimeout_(int stm);

    /** Starts new game */
    void start_();
    /** Continues current game in ChessBrowser */
    void continue_();
    /** Flip the players */
    void flipPlayers_();
    /** haveit for now */
    void pause_();
    /** Player hit resign */
    void resign_();

private:

    /** Return an Engine::SearchSettings struct matching the
        current properties and time. */
    Engine::SearchSettings settings_(int stm) const;

    /** initialize TimeControl at beginning of game.
        If @p beginning is false, continue game instead. */
    void initTiming_(int stm = 0, bool beginning = true);

    /** Start playing board.toMove().
     *  Sets widgets and starts timing.
     *  Sends board to Engine if necessary. */
    void startNewMove_(const SHATRA::Board& board);

    /* Sets the playing side, updates widgets,
     *  optionally starts counting move time */
    //void setSidePlaying_(Color stm, bool do_start_timer);

    /** Updates widgets */
    void setWidgetsPlayer_(int stm);
    /** Updates widgets */
    void setWidgetsPlaying_(bool);
    /** Updates widgets according to who's an Engine */
    void updateEngineWidgets_();

    /** Checks Game result and talks with MainWindow.
        Returns if game ended. */
    bool checkGameResult_(const SHATRA::Board&, bool triggerWinSignals, bool doStop);
    bool isPacked_(int s) { return (s&0x40000000) != 0; }
    bool isWin_(int s) { return (s&0x0100) != 0; }
    bool isLoss_(int s) { return (s&0x0200) != 0; }
    int  distance_(int s) { return s&0xff; }

    /** Sends a comment to gametext with the given info
        (according to properties) */
    void setMoveComment_(int movetime_msec, int score_cp);

    // ---- config ---
    QLed::ledColor colorPlayer_;
    // for blinking when engine thinks
    QLed::ledColor colorEngine0_;
    QLed::ledColor colorEngine1_;
    /** Engine thinking blinking interval in ms */
    int blinkInterval_;

    // ___ MEMBER ___

    Ui::PlayGame * ui_;

    EngineDebugWidget * engineDebug_;

    QTimer blinkTimer_;
    int activeLed_;

    PlayGame * play_;

    SHATRA::Color curStm_;
    bool userMoved_;
    int winStm_;
    QList<SHATRA::Move> plyQue_;
    QVector<quint64> hashHistory_;

    bool playing_,
    /* Player is within a multi capture sequence */
        //playerMultiPly_,
    /** Ignore the next answer from Engine */
        ignoreAnswer_,
    /** Flag used when first player is Engine, sends sendBoard_ */
        sendBoardWhenReady_;
    SHATRA::Board sendBoard_;

    /** last move count in a game */
    int lastMoveNumber_;

    // ---- time control ----

    PlayTimeControl tc_;

    /** Move (incremented when White's turn) */
    int timeMove_,
        timeStm_,
        totalTime_[2],
        moveTime_[2],
        score_[2];

};

#endif // PLAYERSELECT_H
