/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2005-2009 by Michal Rudolf mrudolf@kdewebdev.org                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include "common.h"
#include "historylist.h"
#include "move.h"
#include "output.h"

#include <QtGui>
#include <QAction>
#include <QMainWindow>

namespace SHATRA { class Board; }
class Analysis;
class AnalysisWidget;
class BoardView;
class ChessBrowser;
class Database;
class DatabaseInfo;
class DatabaseList;
class EditAction;
class EngineDebugWidget;
class EventListWidget;
class Game;
class GameList;
class OpeningTree;
class PlayerListWidget;
class PlayGameWidget;
class QLabel;
class QProgressBar;
class QSplitter;
class QTimer;
class QToolBar;
class SaveDialog;
class Statistics;
class TableView;
class ToolMainWindow;

/*
The MainWindow class represents main Shatrabase window. */

class MainWindow: public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

    static const int MaxRecentFiles = 10;

protected:
    /* QObjects Eventfilter for QApplication events */
	bool eventFilter(QObject *obj, QEvent *event);
    /* Overridden to ask for confirmation */
	void closeEvent(QCloseEvent* e);
    /* Filter key events. */
	void keyPressEvent(QKeyEvent *e);

    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);

    /* @return active database */
	Database* database();
    /* @return active database structure */
	DatabaseInfo* databaseInfo();
    /* @return database name of database @p index (or active, if no parameter). */
	QString databaseName(int index = -1) const;
    /* @return active database structure */
	const DatabaseInfo* databaseInfo() const;
    /* @return active game */
	Game& game();
    const Game& game() const;
    /* @return index of active game */
	int gameIndex() const;
    /* Edit comment */
	bool gameEditComment(Output::CommentType type);
    /* Get export filename*/
	QString exportFileName(int& format);
    /* Load game @p index. If @p force is false, does nothing for incorrect @p index .
	With @p force set, creates empty game. */
	void gameLoad(int index, bool force = false, bool reload=0);
    /* Make given number of moves in current game */
	bool gameMoveBy(int change);
    /* Update recent files menu */
	void updateMenuRecent();
    /* Update recent files menu */
	void updateMenuDatabases();
    /* Save Game dialog (created when used first) */
	SaveDialog* saveDialog();

public slots:
    /* Open database */
    void openDatabase(QString fname);
    /* Open database from URL*/
    void openDatabaseUrl(QString fname, bool utf8);
    /* Open a list of databases from a ZIP archive */
    void openDatabaseArchive(QString fname, bool utf8);
    /* Open database from a local File */
    void openDatabaseFile(QString fname, bool utf8);
    /* Add favorite status to a database */
    void setFavoriteDatabase(QString fname);
    /* Open Player dialog */
    void slotPlayerListWidget();
    /* Open Event List */
    void slotEventListWidget();
    /* Promote current variation. */
	void slotEditVarPromote();
    /* Remove current variation. */
	void slotEditVarRemove();
    /* Show Setup Position dialog and set current board. */
	void slotEditBoard();
    /* Edit current comment. */
	void slotEditComment();
    /* Edit current comment. */
    void slotEditCommentBefore();
    /* Set position's SPN to clipboard. */
	void slotEditCopySPN();
    /* Set position using SPN from clipboard. */
	void slotEditPasteSPN();
    /* Set position's SGN to clipboard. */
    void slotEditCopySGN();
    /* Set position using SGN from clipboard. */
    void slotEditPasteSGN();
    /*Set position's Image to clipboard. */
    void slotEditCopyImage();
    /*Truncate moves to the end of the game. */
	void slotEditTruncateEnd();
    void slotEditRemoveResult();
    /*Truncate moves from the beginning of the game. */
	void slotEditTruncateStart();
	void slotReconfigure();
    /** Executes the Preferences */
	void slotConfigure();
    /** Executes the Preferences for Gameplay */
    void slotConfigureGame();
    /* About Shatrabase */
	void slotHelpAbout();
    /* Display Tracker at Shatrabase SourceForge */
	void slotHelpBug();
	void slotConfigureFlip();
    /* Go to first move of the game */
	void slotGameMoveFirst()  {gameMoveBy(-999);}
    /* Go to next move of the game */
    bool slotGameMoveNext();
    /* Go to previous move of the game */
	void slotGameMovePrevious() {gameMoveBy(-1);}
    /* Go to first move of the game */
	void slotGameMoveLast()  {gameMoveBy(999);}
    /* Go N moves forward in the game. For now fixed at @p 10 */
	void slotGameMoveNextN()  {gameMoveBy(10);}
    /* Go N moves backward in the game. For now fixed at @p 10 */
	void slotGameMovePreviousN()  {gameMoveBy(-10);}
    /* Load first game */
	void slotGameLoadFirst();
    /* Load next game */
	void slotGameLoadNext();
    /* Load previous game */
	void slotGameLoadPrevious();
    /* Load last game */
	void slotGameLoadLast();
    /* Load random game */
	void slotGameLoadRandom();
    /* Choose a game and load it */
	void slotGameLoadChosen();
    /* Enter variation. */
	void slotGameVarEnter();
    /* Exit variation. */
	void slotGameVarExit();
    /** Adds a comment after current move */
    void slotGameAddComment(const QString&);
    /* Modify game on user's request. */
	void slotGameModify(const EditAction& action);
    /** Update GameView content when game changed.
     * Also triggers @ref slotMoveChanged, when @p updateMove is true. */
    void slotGameChanged(bool updateMove = true);
    /* Handle link click in Game View panel */
	void slotGameViewLink(const QUrl& link);
    /* Handle link click in Game View panel */
	void slotGameViewLink(const QString& link);
    /* Show/hide GameView source - useful for debugging. */
	void slotGameViewToggle(bool source);
    /* Dump all node information to the console */
    void slotGameDumpMoveNodes();
    /** Create new empty game (to be appended to the database).
        Returns false if the database is readonly or save-current-game was cancelled. */
    bool slotGameNew();
    /** Enable/disable all mainwindow functions not related to gaming.
        if strong is true, additional functionality is taken away. */
    void slotPlayEnableWidgets(bool enable, bool strong = false);
    /** Connected to PlayGameWidget to signal start of new game */
    void slotPlayGameNew(const QMap<QString, QString>& tags);
    /** Connected to PlayGameWidget to signal continuation of current game */
    void slotPlayGameContinue();
    /** Will restore the gui from playing to normal. */
    void slotPlayGamePaused();
    /** Will restore the gui from playing to normal and
        add result to board and possibly save and continue (engine vs. engine). */
    void slotPlayGameEnd();
    /* An Engine/Engine match has finished with a result. */
    //void slotPlayGameEnded();
    /** Connected to PlayGameWidget: Engine has made a move */
    void slotPlayGameMove(SHATRA::Move);
    /** Player has won. */
    void slotPlayPlayerWins();
    /** Not the player has won. */
    void slotPlayOtherWins();
    /** A piece move animation has finished. */
    void slotBoardAnimationFinished(const SHATRA::Board&);
    /* Save game, replacing old one if modified, appending if new
        @return false if cancelled, true if changes are to be saved or discarded. */
    bool slotGameSave();
    /* Add variation to current position. */
	void slotGameAddVariation(const Analysis& analysis);
    /* Add variation to current position. */
    void slotGameAddVariation(const QString& san);
    /* Remove all comments. */
    void slotGameUncomment();
    /* Remove all variations */
    void slotGameRemoveVariations();
    /* Start / Stop AutoPlay feature */
    void slotToggleAutoPlayer();
    /* Auto analysis */
    void slotToggleAutoAnalysis();
    /* Auto Play Timeout - make next move! */
    void slotAutoPlayTimeout();
    /* Filter was changed - update status bar information */
	void slotFilterChanged();
    /* Load given game (triggered from Game List) */
	void slotFilterLoad(int index);
    /* Creates an empty shatra database*/
	void slotFileNew();
    /* Open File dialog, choose a database and open it */
	void slotFileOpen();
    /* Open File dialog, choose a database and open it in UTF8 format */
    void slotFileOpenUtf8();
    /* Save database to current file. */
	void slotFileSave();
    /* Save database to new file. */
    void slotFileSaveAs();
    /* Open given database from Recent files list. */
	void slotFileOpenRecent();
    /* Exports games in filter to SGN file. */
	void slotFileExportFilter();
    /* Exports all games to SGN file. */
	void slotFileExportAll();
    /* Close current database. Switches to next active (or clipboard if there is none). */
	void slotFileClose();
    /* Close selected database. */
    void slotFileCloseIndex(int);
    /* Close selected database. */
    void slotFileCloseName(QString fname);
    /* Quit Shatrabase. Ask for confirmation before */
	void slotFileQuit();
    /* Find games matching single tag. */
	void slotSearchTag();
    /* Find current position */
	void slotSearchBoard();
    /* Receives the signal of a search board operation started */
    void slotBoardSearchStarted();
    /* Receives the signal of a search board operation end */
    void slotBoardSearchUpdate();
    /* Reset current filter */
	void slotSearchReset();
    /* Toggle filter */
    void slotToggleFilter();
    /* Reverse current filter */
	void slotSearchReverse();
    /* Make an Update of the game list after the opening tree was updated */
    void slotTreeUpdate();
    /* Indicate start of tree update */
    void slotTreeUpdateStarted();
    /* Show opening tree */
	void slotSearchTree();
    /* Move @p index was selected in Opening Tree. */
	void slotSearchTreeMove(const QModelIndex& index);
    /* Made given move on the board */
    void slotBoardMove(SHATRA::Square from, SHATRA::Square to, int button);
    /* Board square was clicked */
    void slotBoardClick(SHATRA::Square square, int button, QPoint pos);
    /* Browse current game by mouse wheel */
	void slotBoardMoveWheel(int wheel);
    /** Externalizes or takes back the board view. */
    void slotBoardFlipExternal();
    /** Updates menu checked state */
    void slotBoardExternalClosed();
    /* Update GUI after current move was changed. Update BoardView to current board,
	updates GameInfo and highlight current move in GameView. */
	void slotMoveChanged();
    /* Notation style changed in preferences */
    void slotNotationChanged();
    /* Show temporary message. */
	void slotStatusMessage(const QString& msg);
    /* Show progress bar for open file. */
    void slotOperationProgress(int progress);
    /* Change database. */
	void slotDatabaseChange();
    /* Copy games between databases. */
    void slotDatabaseCopy(int preselect = 1);
    /* Copy games between databases. */
    void slotDatabaseCopySingle();
    /* Database was changed - change informations. */
	void slotDatabaseChanged();
    /* Delete current game. */
    void slotDatabaseDeleteGames(const QVector<int>& idxs);
    /* Delete all game in filter. */
	void slotDatabaseDeleteFilter();
    /* Compact database by removing obsolete information. */
	void slotDatabaseCompact();
    /* Slot that updates internal info upon loading a complete db */
    void slotDataBaseLoaded(DatabaseInfo* db);
    /** Find duplicate games in current database */
    void slotDatabaseFindDuplicates();
    /* Fill up the current game (drag request from game list) */
    void slotGetGameData(Game& g);
    /* Copy game from other database by drag'n'drop */
    void copyGame(QString fileName, const Game& game);
    /** Displays the message @p msg in the status bar. */
    void slotDisplayStatusMessage(const QString& msg);
    void slotDisplayPositionInfo(const QString& msg);

    void slotTestPositionBase();
    void slotTestEngineEngine();
protected slots:
    void loadError(QUrl url);
    void loadReady(QUrl url, QString fileName);
    /* Remove Color of the square in m_annotationSquare */
    void slotNoColorSquare();
    /* Colorize the square in m_annotationSquare green */
    void slotGreenSquare();
    /* Colorize the square in m_annotationSquare yellow */
    void slotYellowSquare();
    /* Colorize the square in m_annotationSquare red */
    void slotRedSquare();

protected:
    void copyGame(int target, const Game& game);

signals:
    /* Re-read configuration. */
    void reconfigure();
    /* Main game has been updated. */
    void boardChange(const SHATRA::Board& board);
    /* Current database changed. */
	void databaseChanged(DatabaseInfo* databaseInfo);
    /* Emitted upon finishing a file download */
    void LoadFinished(DatabaseInfo*);
    /* Emitted upon a update for the elapsed / clock time */
    void displayTime(const QString&, SHATRA::Color);

private:
    /* Create single menu action. */
	QAction* createAction(const QString& name, const char* slot, const QKeySequence& key = QKeySequence(),
										const QString& tip = QString(), QAction::MenuRole menuRole = QAction::NoRole );
    /* Create all the menus and actions */
	void setupActions();
    /* Confirm quitting and save modified databases. */
	bool confirmQuit();
    /* Start operation with progress reporting. Shows progress bar. */
	void startOperation(const QString& msg);
    /* Finish operation with progress reporting. Hides progress bar. */
	void finishOperation(const QString& msg);
    /* Cancel operation with progress reporting. Hides progress bar. */
	void cancelOperation(const QString& msg);
    /* Restore the list of recent files */
    void restoreRecentFiles();
    /* Query User and save game if game was modified */
    bool QuerySaveGame();
    /* Save game without query */
    void saveGame();
    /* Load next game without query */
    void loadNextGame();
    /* Save Database without query or progress bar */
    void saveDatabase();
    /* Save Database with query */
    bool QuerySaveDatabase();

	/* Dialogs  */
	GameList* m_gameList;
    //PlayerListWidget* m_playerList;
    PlayGameWidget * m_playGame;
    //EventListWidget* m_eventList;
    DatabaseList* m_databaseList;
    Statistics* m_stats;
	SaveDialog* m_saveDialog;
	/* Main gui parts */
	BoardView* m_boardView;
	QSplitter* m_boardSplitter;
	ChessBrowser* m_gameView;
    TableView* m_openingTreeView;
	QProgressBar* m_progressBar;
    ToolMainWindow* m_gameWindow;
    EngineDebugWidget * m_engineDebug;
    QToolBar* m_gameToolBar;
	/* Status */
    QLabel* m_statusFilter,
          * m_positionInfo;
    QLabel* m_gameTitle;
	/* Menus */
    QMenu * m_menuFile,
          * m_menuEdit,
          * m_menuDatabase,
          * m_menuDatabases,
          * m_menuView,
          * m_menuGame;

	/* Local variables */
	HistoryList m_recentFiles;
    QStringList m_favoriteFiles;
	Output* m_output;
	QList<DatabaseInfo*> m_databases;
	QList<QAction*> m_recentFileActions;
	QList<QAction*> m_databaseActions;
	int m_currentDatabase;
    int m_prevDatabase;
	QActionGroup* m_actions;
	bool m_showSgnSource; // for debugging
	QTime m_operationTime;
    /* Currently updated tree. May be NULL if no updated in progress. */
	OpeningTree* m_openingTree;
	QString m_nagText;
    QTimer* m_autoPlayTimer;
    QAction //* m_toggleFilter,
            * m_ExternalBoardAction,
            * m_saveAction,
            * m_saveAsAction;
    bool m_bGameChange,
         m_nextGameSaveQuick;
    SHATRA::Board m_lastSendBoard;
    AnalysisWidget* m_mainAnalysis, * m_analysis2;
    SHATRA::Board m_AutoInsertLastBoard;
    SHATRA::Square m_annotationSquare;
    QAction* m_autoPlay;
    QAction* m_autoAnalysis;

    // stuff for storing non-maximized state
    QPoint m_oldPos;
    QSize m_oldSize;
};


#endif
