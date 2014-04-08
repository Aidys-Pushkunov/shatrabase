/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2005-2006 William Hoggarth <whoggarth@users.sourceforge.net>      *
 *   (C) 2008-2009 Michal Rudolf <mrudolf@kdewebdev.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <QObject>
#include <QString>
#include <QTextStream>
#include <QProcess>

#include "analysis.h"
#include "board.h"
#include "move.h"
#include "enginelist.h"

#include "engineoptiondata.h"


#if 1
#   define SB_ENGINE_DEBUG(stream_arg__) \
    { QString str__; QDebug deb__(&str__); deb__ << stream_arg__; engineDebug(this, D_Debug, str__); }
#else
#   define SB_ENGINE_DEBUG(unused__) { }
#endif



/**
 *	Provides a simple interface to a shatra engine.
 */

class Engine : public QObject
{
	Q_OBJECT

public:

    enum DebugType
    {
        D_FromEngine,
        D_ToEngine,
        D_Error,
        D_Debug
    };

    /** Constructs an engine with a given path/command, and log stream */
	Engine(const QString& name,
		const QString& command,
        bool  bTestMode,
		const QString& directory = QString(),
		QTextStream* logStream = NULL);

    /** Virtual destructor */
	virtual ~Engine();

    /** Set the stream that the debug output goes to */
	void setLogStream(QTextStream* logStream = NULL);

    /** Launch and initialize engine, fire activated() signal when done.
        If @p wait == true, the function returns when the process is activated. */
    void activate(bool wait = false);

    /** Destroy engine process */
	void deactivate();

    /** Returns whether the engine process is actually running */
    bool isRunning() const;

    /** Returns whether the engine is active or not */
    bool isActive() const;

    /** Analyzes the given position.
        If @p movetime_ms > 0, limit the analysis time to given millisecs.
        If @p max_ply > 0, only the specified number of plies will be searched. */
    virtual bool startAnalysis(const Board& board, int nv = 1,
                               int movetime_ms = 0, int max_ply = 0) = 0;

    /** Stops any analysis */
	virtual void stopAnalysis() = 0;

    /** Returns whether the engine is analyzing or not */
    bool isAnalyzing() const;

    /** Create a new engine, pass index into engine settings list */
    static Engine* newEngine(int index);
    static Engine* newEngine(int index, bool bTestMode);
    static Engine* newEngine(EngineList& engineList, int index, bool bTestMode);

    /** Set number of lines. */
	virtual void setMpv(int mpv);

    virtual bool providesMvp() { return false; }

    /** Returns the descriptive error text */
    static QString processErrorText(QProcess::ProcessError);

signals:
    /** Fired when the engine is activated */
	void activated();

    /** Fired when the engine is deactivated */
	void deactivated();

    /** Fired when the engine is deactivated */
    void error(QProcess::ProcessError);

    /** Fired when analysis starts */
	void analysisStarted();

    /** Fired when analysis stops */
	void analysisStopped();

    /** Fired when the engine has produced some analysis */
	void analysisUpdated(const Analysis& analysis);

    /** Fired when a log item has been written to the log */
	void logUpdated();

    /** Sends debug info out */
    void engineDebug(Engine * engine, Engine::DebugType t, const QString& str);
#if (0)
    /** Sends the raw communication from the engine */
    void commFromEngine(const QString& msg);

    /** Sends the raw communication that was issued to the engine */
    void commToEngine(const QString& msg);

    /** Sends error messages */
    void commError(const QString& msg);
#endif
protected:
    /** Waits the given milliseconds until an output of the engine.
        Returns false if timed-out. */
    virtual bool waitForResponse(int wait_ms = 1000);

    /** Performs any shutdown procedure required by the engine protocol */
	virtual void protocolEnd() = 0;

    /** Processes messages from the engine */
	virtual void processMessage(const QString& message) = 0;

    /** Sends a message to the engine */
	void send(const QString& message);

    /** Sets whether the engine is active or not */
	void setActive(bool active);

    /** Sets whether the engine is analysing or not */
	void setAnalyzing(bool analyzing);

    /** Sends an analysis signal */
	void sendAnalysis(const Analysis& analysis);

	bool m_invertBlack;
	int m_mpv;
    bool m_bTestMode;

private slots:
    /** Receives notification that there is process output to read */
    void pollProcess();

    /** Receives notification that the process has terminated */
	void processExited();

    /** Performs any initialisation required by the engine protocol */
	virtual void protocolStart() = 0;

    /** Processes messages from the engine */
    void processError(QProcess::ProcessError);

public:
    QList<EngineOptionData> m_options;
    OptionValueList m_mapOptionValues;

protected://rivate:
    void deleteProcess_();

    QString m_name;
	QString	m_command;
	QString	m_directory;
	QTextStream* m_logStream;
	QProcess* m_process;
	QTextStream* m_processStream;
	bool m_active;
	bool m_analyzing;
};



#endif // __ENGINE_H__
