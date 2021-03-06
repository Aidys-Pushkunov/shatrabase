/***************************************************************************
 *                           Shatrabase                                    *
 *   (C) 2012-2014 AFKM <francis@afkm.com>                                 *
 *                   derived from ChessX with thanks                       *
 *   (C) 2006-2007 Sean Estabrooks                                         *
 *   (C) 2007-2009 Michal Rudolf <mrudolf@kdewebdev.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ushiengine.h"
#include "enginedata.h"
#include "qmessagebox.h"

USHIEngine::USHIEngine(const QString& name,
			  const QString& command,
              bool bTestMode,
              const QString& directory)
    : Engine(name, command, bTestMode, directory)
{
	m_quitAfterAnalysis = false;
    m_invertBlack = false;

}

bool USHIEngine::startAnalysis(const SHATRA::Board& board, int nv,
                               const SearchSettings & settings)
{
    SB_ENGINE_DEBUG("USHIEngine::startAnalysis("
                    << nv << ","
                    << settings.maxTime << ", "
                    << settings.maxNodes << ", "
                    << settings.maxDepth << ", "
                    << settings.wtime << ", "
                    << settings.btime << ")");

	m_mpv = nv;
    if (!isActive())
    {
        qDebug() << "*** USHIEngine::startAnalysis() on inactive engine";
		return false;
	}

    //if (m_board == board)
    //	return true;
	m_board = board;

	m_position = board.toSPN();
	m_waitingOn = "ushinewgame";
    m_settings = settings;
	send("stop");
    send("ushinewgame");
    send("isready");
	setAnalyzing(true);

	return true;
}

void USHIEngine::stopAnalysis()
{
	if (isAnalyzing())
		send("stop");
}

void USHIEngine::setMpv(int mpv)
{
    Engine::setMpv(mpv);

    if (isAnalyzing())
    {
		send("stop");

        if (!m_restartOnMpvChange)
        {
            send(QString("setoption name MultiPV value %1").arg(m_mpv));
            send("position spn " + m_position);
            send("go infinite");
        }
        else
        {
            startAnalysis(m_board, m_mpv, m_settings);
        }
	}
}


void USHIEngine::protocolStart()
{
    //tell the engine we are using the ushi protocol
    send("ushi");
}

void USHIEngine::protocolEnd()
{
	send("quit");
	setActive(false);
	m_board.clear();
}

void USHIEngine::setShatraVersion()
{
    send(QString("setoption name Shatra version value %1").arg(SHATRA::g_version));
}

void USHIEngine::processMessage(const QString& message)
{
    qDebug()<<message;
    if (message == "ushiok") {
        setShatraVersion();
		//once the engine is running wait for it to initialise
        m_waitingOn = "ushiok";
        send("isready");
	}

    if (message == "version unsupported")
    {
        protocolEnd();
        QMessageBox mb;
        mb.setText(tr("The engine does not support this version of Shatra"));
        mb.exec();
        // quit
    }
    if (message == "version supported")
    {
        m_waitingOn = "ushiok";
//        send("isready");
    }

    if (message == "readyok")
    {
		if (m_waitingOn == "ushiok") {
			//engine is now initialised and ready to go
			m_waitingOn = "";
			setActive(true);

            if (!m_bTestMode)
            {
                OptionValueList::const_iterator i = m_mapOptionValues.constBegin();
                while (i != m_mapOptionValues.constEnd()) {
                    QString key = i.key();
                    QVariant value = i.value();
                    if (EngineOptionData* dataSpec = EngineOptionData::FindInList(key, m_options))
                    {
                        switch (dataSpec->m_type)
                        {
                        case OPT_TYPE_BUTTON:
                            if (value.toBool())
                            {
                                send(QString("setoption name %1 value %2").arg(key));
                            }
                            break;
                        case OPT_TYPE_CHECK:
                        case OPT_TYPE_SPIN:
                        case OPT_TYPE_STRING:
                        case OPT_TYPE_COMBO:
                            if (dataSpec->m_defVal != value.toString() && !value.toString().isEmpty())
                            {
                                send(QString("setoption name %1 value %2").arg(key).arg(value.toString()));
                            }
                            break;
                        }
                    }
                    ++i;
                }
            }

            // XXX below now
            //send("setoption name USHI_AnalyseMode value true");

            emit readyOk();
		}

        if (m_waitingOn == "ushinewgame")
        {
            //engine is now ready to analyse a new position
			m_waitingOn = "";
			send(QString("setoption name MultiPV value %1").arg(m_mpv));
			send("position spn " + m_position);

            // -- construct go command --

            QString cmd("go");
            if (m_settings.maxNodes != SearchSettings::Unlimited)
                cmd += QString(" nodes %1").arg(m_settings.maxNodes);
            if (m_settings.maxDepth != SearchSettings::Unlimited)
                cmd += QString(" depth %1").arg(m_settings.maxDepth);
            if (m_settings.maxTime != SearchSettings::Unlimited)
                cmd += QString(" movetime %1").arg(m_settings.maxTime);
            if (m_settings.btime != SearchSettings::Unlimited)
                cmd += QString(" btime %1").arg(m_settings.btime);
            if (m_settings.btime != SearchSettings::Unlimited)
                cmd += QString(" wtime %1").arg(m_settings.wtime);
            if (m_settings.movestogo != SearchSettings::Unlimited)
                cmd += QString(" movestogo %1").arg(m_settings.movestogo);
            if (m_settings.isUnlimited())
            {
                cmd += " infinite";
                send("setoption name USHI_AnalyseMode value true");
            }
            else
                send("setoption name USHI_AnalyseMode value false");
            send(cmd);
        }
	}

    QString command = message.section(' ', 0, 0);

    if (isAnalyzing())
    {
        if (command == "info")
            parseAnalysis(message);
        else if (command == "bestmove")
            parseBestmove(message);
    }
    else
    {
        if (command == "option")
            parseOptions(message);
    }
}

void USHIEngine::parseBestmove(const QString& message)
{
    QString info = message.section(' ', 1, -1, QString::SectionSkipEmpty);
    QString moveText = info.section(' ', 0, 0, QString::SectionSkipEmpty);

    SHATRA::Board board(m_board);
    SHATRA::Move move = board.parseMove(moveText);
    if (!move.isLegal())
    {
        engineDebug(this, D_Error, tr("illegal bestmove '%1' from engine!").arg(moveText));
        return;
    }

    emit bestMoveSend(move);

    /*board.doMove(move);

    MoveList moves;
    moves.append(move);

    Analysis analysis;
    analysis.setVariation(moves);
    sendAnalysis(analysis);
    */
}

void USHIEngine::parseAnalysis(const QString& message)
{
	Analysis analysis;
    bool multiPVFound, timeFound, nodesFound, npsFound, depthFound, scoreFound, variationFound;
    multiPVFound = timeFound = nodesFound = npsFound = depthFound = scoreFound = variationFound = false;

	QString info = message.section(' ', 1, -1, QString::SectionSkipEmpty);
	int section = 0;
	QString name;
	bool ok;

	//loop around the name value tuples
    while (!info.section(' ', section, section + 1, QString::SectionSkipEmpty).isEmpty())
    {
		name = info.section(' ', section, section, QString::SectionSkipEmpty);
//        qDebug() << "name " << name;
		if (name == "multipv") {
			analysis.setNumpv(info.section(' ', section + 1, section + 1, QString::SectionSkipEmpty).toInt(&ok));
			section += 2;
			if (ok) {
				multiPVFound = true;
				continue;
			}
		}

		if (name == "time") {
			analysis.setTime(info.section(' ', section + 1, section + 1, QString::SectionSkipEmpty).toInt(&ok));
			section += 2;
			if (ok) {
				timeFound = true;
				continue;
			}
		}

		if (name == "nodes") {
            analysis.setNodes(info.section(' ', section + 1, section + 1, QString::SectionSkipEmpty).toLongLong(&ok));
			section += 2;
			if (ok) {
				nodesFound = true;
				continue;
			}
		}
        if (name == "nps") {
            analysis.setNodesPerSecond(info.section(' ', section + 1, section + 1, QString::SectionSkipEmpty).toLongLong(&ok));
            section += 2;
            if (ok) {
                npsFound = true;
                continue;
            }
        }

        if (name == "depth")
        {
			analysis.setDepth(info.section(' ', section + 1, section + 1, QString::SectionSkipEmpty).toInt(&ok));
			section += 2;
			if (ok) {
				depthFound = true;
				continue;
			}
		}

        if (name == "score")
        {
			QString type = info.section(' ', section + 1, section + 1, QString::SectionSkipEmpty);

            // check if pure number
            int score = type.toDouble(&ok);
            if (ok)
            {
                if (m_invertBlack && m_board.toMove() == SHATRA::Black)
                    analysis.setScore(-score);
                else
                    analysis.setScore(score);
                scoreFound = true;
            }
            else
            if (type == "cp" || type == "win" || type == "loss") {
                score = info.section(' ', section + 2, section + 2).toInt(&ok);

                if(type == "win")
                {
                    analysis.setMovesToResult(abs(score), SHATRA::Win);
                }
                else if(type == "loss")
                {
                    analysis.setMovesToResult(abs(score), SHATRA::Loss);
                }
                else if (m_invertBlack && m_board.toMove() == SHATRA::Black)
                {
					analysis.setScore(-score);
                }
				else analysis.setScore(score);

				section += 3;
				if (ok) {
					scoreFound = true;
					continue;
				}
			}
			else section += 3;
		}

        if (name == "pv")
        {
            SHATRA::Board board = m_board;
            SHATRA::MoveList moves;
			QString moveText;
			section++;
            while ((moveText = info.section(' ', section, section, QString::SectionSkipEmpty)) != "")
            {
//                qDebug() << "pv " << moveText;
                SHATRA::Move move = board.parseMove(moveText);
				if (!move.isLegal())
                {
                    engineDebug(this, D_Error, tr("illegal move '%1' from engine!").arg(moveText));
					break;
                }
				board.doMove(move);
				moves.append(move);
				section++;
			}
			analysis.setVariation(moves);
		}

		//not understood, skip
		section += 2;
	}

	if (timeFound && nodesFound && scoreFound && analysis.isValid()) {
		if (!multiPVFound)
			analysis.setNumpv(1);
		sendAnalysis(analysis);
	}
}

void USHIEngine::parseOptions(const QString& message)
{
    enum ScanPhase { EXPECT_OPTION,
                     EXPECT_NAME,
                     EXPECT_TYPE_TOKEN,
                     EXPECT_TYPE,
                     EXPECT_DEFAULT_VALUE,
                     EXPECT_MIN_MAX_DEFAULT,
                     EXPECT_MIN_VALUE,
                     EXPECT_MAX_VALUE,
                     EXPECT_VAR_TOKEN,
                     EXPECT_VAR } phase;

    phase = EXPECT_OPTION;
    QStringList list = message.split(QRegExp("\\W+"), QString::SkipEmptyParts);

    QStringList nameVals;
    QString defVal;
    QString minVal;
    QString maxVal;
    QStringList varVals;
    OptionType optionType = OPT_TYPE_STRING;
    QString error;
    bool done = false;
    foreach (QString token, list)
    {
        switch (phase)
        {
        case EXPECT_OPTION:
            if (token == "option")
                phase = EXPECT_NAME;
            else
                error = token;
            break;
        case EXPECT_NAME:
            if (token == "name")
                phase = EXPECT_TYPE;
            else
                error = token;
            break;
        case EXPECT_TYPE:
            if (token == "type")
                phase = EXPECT_TYPE_TOKEN;
            else
                nameVals << token;
            break;
        case EXPECT_TYPE_TOKEN:
            if (token == "check")
                optionType = OPT_TYPE_CHECK;
            else if (token == "spin")
                optionType = OPT_TYPE_SPIN;
            else if (token == "combo")
                optionType = OPT_TYPE_COMBO;
            else if (token == "button")
            {
                optionType = OPT_TYPE_BUTTON;
                done = true;
            }
            else if (token == "string")
                optionType = OPT_TYPE_STRING;
            else error = token;

            phase = EXPECT_MIN_MAX_DEFAULT;
            break;
        case EXPECT_DEFAULT_VALUE:
            defVal = token;
            switch (optionType)
            {
            case OPT_TYPE_SPIN:
                phase = EXPECT_MIN_MAX_DEFAULT;
                break;
            case OPT_TYPE_COMBO:
                phase = EXPECT_VAR_TOKEN;
                break;
            case OPT_TYPE_CHECK:
            case OPT_TYPE_STRING:
            default:
                done = true;
                break;
            }
            break;
        case EXPECT_MIN_MAX_DEFAULT:
            if (token == "default")
                phase = EXPECT_DEFAULT_VALUE;
            else if (token == "min")
                phase = EXPECT_MIN_VALUE;
            else if (token == "max")
                phase = EXPECT_MAX_VALUE;
            else
                done = true;
            break;
        case EXPECT_MIN_VALUE:
            minVal = token;
            phase = EXPECT_MIN_MAX_DEFAULT;
            break;
        case EXPECT_MAX_VALUE:
            maxVal = token;
            phase = EXPECT_MIN_MAX_DEFAULT;
            break;
        case EXPECT_VAR_TOKEN:
            if (token == "var")
                phase = EXPECT_VAR;
            else
                done = true;
            break;
        case EXPECT_VAR:
            varVals << token;
            phase = EXPECT_VAR_TOKEN;
            break;
        default:
            error = token;
            return;
        }

        if (done || !error.isEmpty())
        {
            break;
        }
    }
    if (!error.isEmpty())
    {
        engineDebug(this, D_Error, tr(
                 "Cannot parse Option string: '%1' "
                 "looking at token '%2'!")
                 .arg(message).arg(error));
        return;
    }
    if (done || (phase > EXPECT_DEFAULT_VALUE))
    {
        QString name = nameVals.join(" ");
        EngineOptionData option;
        option.m_name = name;
        option.m_minVal = minVal;
        option.m_maxVal = maxVal;
        option.m_defVal = defVal;
        option.m_varVals = varVals;
        option.m_type = optionType;

        m_options.append(option);
    }
    else
    {
        engineDebug(this, D_Error, tr(
                    "Incomplete syntax parsing Option string: '%1' !")
                    .arg(message));
        return;
    }
}
