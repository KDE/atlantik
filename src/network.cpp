#include <kdebug.h>

#include "network.moc"
#include "atlantik.h"

GameNetwork::GameNetwork(Atlantik *parent, const char *name) : QSocket(parent, name)
{
	m_parentWindow = parent;
	connect(this, SIGNAL(readyRead()), this, SLOT(slotRead()));
}

void GameNetwork::roll()
{	writeData(".r");
}

void GameNetwork::buyEstate()
{	writeData(".eb");
}

void GameNetwork::startGame()
{
	writeData(".gs");
}

void GameNetwork::endTurn()
{	writeData(".E");
}

void GameNetwork::cmdName(QString name)
{
	QString msg(".n");
	msg.append(name);
	writeData(msg);
}

void GameNetwork::cmdTokenConfirmation(int estateId)
{
	QString msg(".t");
	msg.append(QString::number(estateId));
	writeData(msg);
}

void GameNetwork::estateToggleMortgage(int estateId)
{
	QString msg(".em");
	msg.append(QString::number(estateId));
	writeData(msg);
}

void GameNetwork::estateHouseBuy(int estateId)
{
	QString msg(".hb");
	msg.append(QString::number(estateId));
	writeData(msg);
}

void GameNetwork::estateHouseSell(int estateId)
{
	QString msg(".hs");
	msg.append(QString::number(estateId));
	writeData(msg);
}

void GameNetwork::cmdGamesList()
{	writeData(".gl");
}

void GameNetwork::newGame()
{
	writeData(".gn");
}

void GameNetwork::joinGame(int gameId)
{
	QString msg(".gj");
	msg.append(QString::number(gameId));
	writeData(msg);
}

void GameNetwork::cmdChat(QString msg)
{	writeData(msg);
}

void GameNetwork::cmdTradeNew()
{	writeData(".Tn");
}

void GameNetwork::cmdTradeToggleEstate(int tradeId, int estateId)
{
	QString msg(".Te");
	msg.append(QString::number(estateId));
	msg.append(":");
	msg.append(QString::number(tradeId));
	writeData(msg);
}

void GameNetwork::cmdTradeSetMoney(int tradeId, int amount)
{
	QString msg(".Tm");
	msg.append(QString::number(amount));
	msg.append(":");
	msg.append(QString::number(tradeId));
	writeData(msg);
}

void GameNetwork::cmdTradeAccept(int tradeId)
{
	QString msg(".Ta");
	msg.append(QString::number(tradeId));
	writeData(msg);
}

void GameNetwork::cmdTradeReject(int tradeId)
{
	QString msg(".Tr");
	msg.append(QString::number(tradeId));
	writeData(msg);
}

void GameNetwork::jailPay()
{
	writeData(".jp");
}

void GameNetwork::jailRoll()
{
	writeData(".jr");
}

void GameNetwork::jailCard()
{
	writeData(".jc");
}

void GameNetwork::writeData(QString msg)
{
	msg.append("\n");
	if (state()==QSocket::Connection)
	{
		kdDebug() << "out [" << msg << "]" << endl;
		writeBlock(msg.latin1(), strlen(msg.latin1()));
	}
	else
		kdDebug() << "not [" << msg << "]" << endl;
}

void GameNetwork::slotRead()
{
	while(canReadLine())
		processMsg(readLine());

	// Maximum message size. Messages won't get bigger than 32k anyway, so
	// if we didn't receive a newline by now, we probably won't anyway.
	if (bytesAvailable() > (1024 * 32))
		flush();
}

void GameNetwork::processMsg(QString str)
{
	kdDebug() << "processing msg: " + str << endl;
	msg.setContent(str);
	QDomElement e = msg.documentElement();
	if (e.tagName() != "monopd")
	{
		kdDebug() << "invalid message: " << str << endl;
		return;
	}
	QDomNode n = e.firstChild();
	processNode(n);
}

void GameNetwork::processNode(QDomNode n)
{
	QDomAttr a;

	while(!n.isNull())
	{
		QDomElement e = n.toElement();
		if(!e.isNull())
		{
			if (e.tagName() == "msg")
			{
				a = e.attributeNode(QString("type"));
				if (!a.isNull())
				{
					if (a.value() == "error")
						emit msgError(e.attributeNode(QString("value")).value());
					else if (a.value() == "info")
						m_parentWindow->serverMsgsAppend( e.attributeNode(QString("value")).value() );
					else if (a.value() == "chat")
						emit msgChat(e.attributeNode(QString("author")).value(), e.attributeNode(QString("value")).value());
					else if (a.value() == "startgame")
					{
						emit startedGame();
						emit msgStartGame(e.attributeNode(QString("value")).value());
					}
				}
			}
			else if (e.tagName() == "display")
			{
				QString type = e.attributeNode(QString("type")).value();
				if (type == "chancecard" ||type == "communitychestcard")
					emit displayCard(type, e.attributeNode(QString("description")).value());
			}
			else if (e.tagName() == "updategamelist")
			{
				QString type = e.attributeNode(QString("type")).value();
				if (type == "full")
					emit gameListClear();

				QDomNode n_game = n.firstChild();
				while(!n_game.isNull())
				{
					QDomElement e_game = n_game.toElement();
					if (!e_game.isNull() && e_game.tagName() == "game")
					{
						if (type=="del")
							emit gameListDel(e_game.attributeNode(QString("id")).value());
						else if (type=="edit")
							emit gameListEdit(e_game.attributeNode(QString("id")).value(), e_game.attributeNode(QString("description")).value(), e_game.attributeNode(QString("players")).value());
						else if (type=="add" || type=="full")
							emit gameListAdd(e_game.attributeNode(QString("id")).value(), e_game.attributeNode(QString("description")).value(), e_game.attributeNode(QString("players")).value());
					}
					n_game = n_game.nextSibling();
				}
				emit gamelistEndUpdate(type);
			}
			else if (e.tagName() == "joinedgame")
			{
				emit joinedGame();
			}
			else if (e.tagName() == "updateplayerlist")
			{
				QString type = e.attributeNode(QString("type")).value();
				if (type == "full")
					emit playerListClear();

				QDomNode n_player = n.firstChild();
				while(!n_player.isNull())
				{
					QDomElement e_player = n_player.toElement();
					if (!e_player.isNull() && e_player.tagName() == "player")
					{
						if (type=="del")
							emit playerListDel(e_player.attributeNode(QString("clientid")).value());
						else if (type=="edit")
							emit playerListEdit(e_player.attributeNode(QString("clientid")).value(), e_player.attributeNode(QString("name")).value(), e_player.attributeNode(QString("host")).value());
						else if (type=="add" || type=="full")
						{
							kdDebug() << "emit GameNetwork::playerListAdd" << endl;
							emit playerListAdd(e_player.attributeNode(QString("clientid")).value(), e_player.attributeNode(QString("name")).value(), e_player.attributeNode(QString("host")).value());
						}
					}
					n_player = n_player.nextSibling();
				}
				emit playerListEndUpdate(type);
			}
			else if (e.tagName() == "client")
			{
				a = e.attributeNode(QString("playerid"));
				if (!a.isNull())
					emit setPlayerId(a.value().toInt());
			}
			else if (e.tagName() == "newturn")
			{
				// Find out which player has the turn now
				int player = e.attributeNode(QString("player")).value().toInt();
				emit setTurn(player);
			}
			else if (e.tagName() == "gameupdate")
			{
				int gameId = -1;

				a = e.attributeNode(QString("gameid"));
				if (!a.isNull())
				{
					gameId = a.value().toInt();

					QString status = e.attributeNode(QString("status")).value();
					if (status == "init")
						emit initGame();
				}
			}
			else if (e.tagName() == "playerupdate")
			{
				int playerId = -1, location = -1;
				bool directmove = false;

				a = e.attributeNode(QString("playerid"));
				if (!a.isNull())
				{
					playerId = a.value().toInt();

					// Create player object and view
					emit playerInit(playerId);

					// Update player name
					a = e.attributeNode(QString("name"));
					if (!a.isNull())
						emit msgPlayerUpdateName(playerId, a.value());

					// Update player money
					a = e.attributeNode(QString("money"));
					if (!a.isNull())
						emit msgPlayerUpdateMoney(playerId, a.value());

					a = e.attributeNode(QString("jailed"));
					if (!a.isNull())
						emit msgPlayerUpdateJailed(playerId, a.value());

					// Update player location
					a = e.attributeNode(QString("location"));
					if (!a.isNull())
						location = a.value().toInt();

					if (location != -1)
					{
						a = e.attributeNode(QString("directmove"));
						if (!a.isNull())
							directmove = a.value().toInt();

						emit msgPlayerUpdateLocation(playerId, location, directmove);
					}

					kdDebug() << "emit playerUpdateFinished(" << playerId << ")" << endl;
					emit playerUpdateFinished(playerId);
				}
			}
			else if (e.tagName() == "estateupdate")
			{
				kdDebug() << "ESTATEUPDATE" << endl;

				int estateId = -1;

				a = e.attributeNode(QString("estateid"));
				if (!a.isNull())
				{
					estateId = a.value().toInt();
					kdDebug() << "ESTATEUPDATE id " << estateId << endl;

					// Create estate object and view
					kdDebug() << "emit estateInit(" << estateId << ")" << endl;
					emit estateInit(estateId);

					a = e.attributeNode(QString("name"));
					if (!a.isNull())
						emit msgEstateUpdateName(estateId, a.value());

					a = e.attributeNode(QString("color"));
					if (!a.isNull() && !a.value().isEmpty())
						emit msgEstateUpdateColor(estateId, a.value());

					a = e.attributeNode(QString("bgcolor"));
					if (!a.isNull())
						emit msgEstateUpdateBgColor(estateId, a.value());

					a = e.attributeNode(QString("owner"));
					if (!a.isNull())
						emit msgEstateUpdateOwner(estateId, a.value().toInt());

					a = e.attributeNode(QString("houses"));
					if (!a.isNull())
						emit msgEstateUpdateHouses(estateId, a.value().toInt());

					a = e.attributeNode(QString("mortgaged"));
					if (!a.isNull())
						emit msgEstateUpdateMortgaged(estateId, a.value().toInt());

					a = e.attributeNode(QString("groupid"));
					if (!a.isNull())
						emit msgEstateUpdateGroupId(estateId, a.value().toInt());

					a = e.attributeNode(QString("can_toggle_mortgage"));
					if (!a.isNull())
						emit msgEstateUpdateCanToggleMortgage(estateId, a.value().toInt());

					a = e.attributeNode(QString("can_be_owned"));
					if (!a.isNull())
						emit msgEstateUpdateCanBeOwned(estateId, a.value().toInt());

					a = e.attributeNode(QString("can_buy_houses"));
					if (!a.isNull())
						emit estateUpdateCanBuyHouses(estateId, a.value().toInt());

					a = e.attributeNode(QString("can_sell_houses"));
					if (!a.isNull())
						emit estateUpdateCanSellHouses(estateId, a.value().toInt());

					kdDebug() << "emit estateUpdateFinished(" << estateId << ")" << endl;
					emit estateUpdateFinished(estateId);
				}
			}
			else if (e.tagName() == "tradeupdate")
			{
				a = e.attributeNode(QString("tradeid"));
				if (!a.isNull())
				{
					int tradeId = a.value().toInt();
					kdDebug() << "TRADEUPDATE id " << tradeId << endl;

					// Create trade object and view
					kdDebug() << "emit tradeInit(" << tradeId << ")" << endl;
					emit tradeInit(tradeId);

					QString type = e.attributeNode(QString("type")).value();

					if (type=="new")
					{
						emit tradeUpdateActor(tradeId, e.attributeNode(QString("actor")).value().toInt());

						QDomNode n_player = n.firstChild();
						while(!n_player.isNull())
						{
							QDomElement e_player = n_player.toElement();
							if (!e_player.isNull() && e_player.tagName() == "tradeplayer")
							{
								kdDebug() << "emit tradeUpdatePlayerAdd" << endl;
								emit tradeUpdatePlayerAdd(tradeId, e_player.attributeNode(QString("playerid")).value().toInt());
							}
							n_player = n_player.nextSibling();
						}
					}
					else if (type=="edit")
					{
						QDomNode n_child = n.firstChild();
						while(!n_child.isNull())
						{
							QDomElement e_child = n_child.toElement();
							if (!e_child.isNull())
							{
								if (e_child.tagName() == "tradeplayer")
								{
									a = e.attributeNode(QString("playerid"));
									if (!a.isNull())
									{
										int playerId = a.value().toInt();

										a = e.attributeNode(QString("accept"));
										if (!a.isNull())
											emit msgTradeUpdatePlayerAccept(tradeId, playerId, (bool)(a.value().toInt()));

										a = e.attributeNode(QString("money"));
										if (!a.isNull())
											emit msgTradeUpdatePlayerMoney(tradeId, playerId, a.value().toInt());
									}
								}
								else if (e_child.tagName() == "tradeestate")
								{
									a = e.attributeNode(QString("estateid"));
									if (!a.isNull())
									{
										int estateId = a.value().toInt();

										a = e.attributeNode(QString("included"));
										if (!a.isNull())
											emit msgTradeUpdateEstateIncluded(tradeId, estateId, (bool)(a.value().toInt()));
									}
								}
							}
							n_child = n_child.nextSibling();
						}
					}
					else if (type=="accepted")
						emit msgTradeUpdateAccepted(tradeId);
					else if (type=="completed")
						emit msgTradeUpdateCompleted(tradeId);
					else if (type=="rejected")
						emit msgTradeUpdateRejected(tradeId, e.attributeNode(QString("actor")).value().toInt());

					emit tradeUpdateFinished(tradeId);
				}
			}
			else
				kdDebug() << "ignored TAG: " << e.tagName() << endl;
		}
		QDomNode node = n.firstChild();
		processNode(node);
		n = n.nextSibling();
	}
}

void GameNetwork::serverConnect(const QString host, int port)
{
	connectToHost(host, port);
}
