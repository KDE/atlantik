#include <kdebug.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <trade.h>
#include <auction.h>

#include "atlantik_network.moc"

//#include "atlantik.h"
//#include "trade_widget.h"

#ifndef USE_KDE
AtlantikNetwork::AtlantikNetwork(AtlanticCore *atlanticCore, QObject *parent, const char *name) : QSocket(parent, name)
#else
AtlantikNetwork::AtlantikNetwork(AtlanticCore *atlanticCore, QObject *parent, const char *name) : KExtendedSocket(0, 0, KExtendedSocket::inputBufferedSocket)
#endif
{
	m_atlanticCore = atlanticCore;
	m_parent = parent;
	m_clientId = m_playerId = -1;

	QObject::connect(this, SIGNAL(readyRead()), this, SLOT(slotRead()));
}

void AtlantikNetwork::roll()
{	writeData(".r");
}

void AtlantikNetwork::buyEstate()
{
	writeData(".eb");
}

void AtlantikNetwork::auctionEstate()
{
	writeData(".ea");
}

void AtlantikNetwork::startGame()
{
	writeData(".gs");
}

void AtlantikNetwork::endTurn()
{	writeData(".E");
}

void AtlantikNetwork::cmdName(QString name)
{
	QString msg(".n");
	msg.append(name);
	writeData(msg);
}

void AtlantikNetwork::tokenConfirmation(Estate *estate)
{
	QString msg(".t");
	msg.append(QString::number(estate ? estate->estateId() : -1));
	writeData(msg);
}

void AtlantikNetwork::estateToggleMortgage(Estate *estate)
{
	QString msg(".em");
	msg.append(QString::number(estate ? estate->estateId() : -1));
	writeData(msg);
}

void AtlantikNetwork::estateHouseBuy(Estate *estate)
{
	QString msg(".hb");
	msg.append(QString::number(estate ? estate->estateId() : -1));
	writeData(msg);
}

void AtlantikNetwork::estateHouseSell(Estate *estate)
{
	QString msg(".hs");
	msg.append(QString::number(estate ? estate->estateId() : -1));
	writeData(msg);
}

void AtlantikNetwork::cmdGamesList()
{	writeData(".gl");
}

void AtlantikNetwork::newGame(const QString &gameType)
{
	writeData(".gn" + gameType);
}

void AtlantikNetwork::joinGame(int gameId)
{
	QString msg(".gj");
	msg.append(QString::number(gameId));
	writeData(msg);
}

void AtlantikNetwork::cmdChat(QString msg)
{	writeData(msg);
}

void AtlantikNetwork::newTrade(Player *player)
{
	QString msg(".Tn");
	msg.append(QString::number(player ? player->playerId() : -1));
	writeData(msg);
}

void AtlantikNetwork::tradeUpdateEstate(Trade *trade, Estate *estate, Player *player)
{
	QString msg(".Te");
	msg.append(QString::number(trade ? trade->tradeId() : -1));
	msg.append(":");
	msg.append(QString::number(estate ? estate->estateId() : -1));
	msg.append(":");
	msg.append(QString::number(player ? player->playerId() : -1));
	writeData(msg);
}

void AtlantikNetwork::tradeUpdateMoney(Trade *trade, unsigned int money, Player *pFrom, Player *pTo)
{
	QString msg(".Tm");
	msg.append(QString::number(trade ? trade->tradeId() : -1));
	msg.append(":");
	msg.append(QString::number(pFrom ? pFrom->playerId() : -1));
	msg.append(":");
	msg.append(QString::number(pTo ? pTo->playerId() : -1));
	msg.append(":");
	msg.append(QString::number(money));
	writeData(msg);
}

void AtlantikNetwork::tradeReject(Trade *trade)
{
	QString msg(".Tr");
	msg.append(QString::number(trade ? trade->tradeId() : -1));
	writeData(msg);
}

void AtlantikNetwork::cmdTradeAccept(int tradeId)
{
	QString msg(".Ta");
	msg.append(QString::number(tradeId));
	writeData(msg);
}


void AtlantikNetwork::auctionBid(Auction *auction, int amount)
{
	QString msg(".ab");
	msg.append(QString::number(auction ? auction->auctionId() : -1));
	msg.append(":");
	msg.append(QString::number(amount));
	writeData(msg);
}

void AtlantikNetwork::jailPay()
{
	writeData(".jp");
}

void AtlantikNetwork::jailRoll()
{
	writeData(".jr");
}

void AtlantikNetwork::jailCard()
{
	writeData(".jc");
}

void AtlantikNetwork::writeData(QString msg)
{
	msg.append("\n");
#ifndef USE_KDE
	if (state()==QSocket::Connection)
#else
	if (socketStatus()==KExtendedSocket::connected)
#endif
	{
		kdDebug() << "out [" << msg << "]" << endl;
		writeBlock(msg.latin1(), strlen(msg.latin1()));
	}
	else
		kdDebug() << "not [" << msg << "]" << endl;
}

void AtlantikNetwork::slotRead()
{
	kdDebug() << "slotRead" << endl;
#ifndef USE_KDE
	while(canReadLine())
		processMsg(readLine());
#else
	char *tmp = NULL;
	while(canReadLine())
	{
		kdDebug() << "canReadLine!" << endl;
		readLine(tmp, 1024 * 32);
		processMsg(tmp);
	}
#endif

	// Maximum message size. Messages won't get bigger than 32k anyway, so
	// if we didn't receive a newline by now, we probably won't anyway.
	if (bytesAvailable() > (1024 * 32))
		flush();
}

void AtlantikNetwork::processMsg(QString str)
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

void AtlantikNetwork::processNode(QDomNode n)
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
						emit msgInfo(e.attributeNode(QString("value")).value());
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
							emit gameListEdit(e_game.attributeNode(QString("id")).value(), e_game.attributeNode(QString("gametype")).value(), e_game.attributeNode(QString("description")).value(), e_game.attributeNode(QString("players")).value());
						else if (type=="add" || type=="full")
							emit gameListAdd(e_game.attributeNode(QString("id")).value(), e_game.attributeNode(QString("gametype")).value(), e_game.attributeNode(QString("description")).value(), e_game.attributeNode(QString("players")).value());
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
							int playerId = -1;
							a = e_player.attributeNode(QString("playerid"));
							if (!a.isNull())
								playerId = a.value().toInt();

							a = e_player.attributeNode(QString("clientid"));
							if (!a.isNull() && a.value().toInt() == m_clientId)
								m_playerId = playerId;
							
							emit playerListAdd(e_player.attributeNode(QString("clientid")).value(), e_player.attributeNode(QString("name")).value(), e_player.attributeNode(QString("host")).value());
						}
					}
					n_player = n_player.nextSibling();
				}
				emit playerListEndUpdate(type);
			}
			else if (e.tagName() == "client")
			{
				a = e.attributeNode(QString("clientid"));
				if (!a.isNull())
					m_clientId = a.value().toInt();
			}
			else if (e.tagName() == "newturn")
			{
				Player *player = m_players[e.attributeNode(QString("player")).value().toInt()];
				if (player)
					// Update *all* objects
					m_atlanticCore->setCurrentTurn(player);
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
				int playerId = -1;

				a = e.attributeNode(QString("playerid"));
				if (!a.isNull())
				{
					playerId = a.value().toInt();

					Player *player;
					bool b_newPlayer = false;
					if (!(player = m_players[playerId]))
					{
						// Create player object
						player = m_atlanticCore->newPlayer(playerId);
						m_players[playerId] = player;

						b_newPlayer = true;
					}

					// Check if this is us
					if (player && player->playerId() == m_playerId)
						player->setIsSelf(true);

					// Update player name
					a = e.attributeNode(QString("name"));
					if (player && !a.isNull())
						player->setName(a.value());

					// Update player money
					a = e.attributeNode(QString("money"));
					if (player && !a.isNull())
						player->setMoney(a.value().toInt());

					// Update whether player can roll
					a = e.attributeNode(QString("can_roll"));
					if (player && !a.isNull())
						player->setCanRoll(a.value().toInt());

					// Update whether player can buy
					a = e.attributeNode(QString("can_buyestate"));
					if (player && !a.isNull())
						player->setCanBuy(a.value().toInt());

					// Update whether player is jailed
					a = e.attributeNode(QString("jailed"));
					if (player && !a.isNull())
						player->setInJail(a.value().toInt());

					// Update player location
					a = e.attributeNode(QString("location"));
					if (!a.isNull())
					{
						Estate *estate = m_estates[a.value().toInt()];
						bool directMove = false;

						a = e.attributeNode(QString("directmove"));
						if (!a.isNull())
							directMove = a.value().toInt();

						if (estate)
							player->setLocation(estate);
					}

					// Emit signal so GUI implementations can create view(s)
#warning port to atlanticcore, but somehow dont create view until all properties are set
					if (b_newPlayer)
						emit newPlayer(player);

					if (player)
						player->update();
				}
			}
			else if (e.tagName() == "estateupdate")
			{
				int estateId = -1;

				a = e.attributeNode(QString("estateid"));
				if (!a.isNull())
				{
					estateId = a.value().toInt();
					kdDebug() << "ESTATEUPDATE id " << estateId << endl;

					Estate *estate;
					bool b_newEstate = false;
					if (!(estate = m_estates[a.value().toInt()]))
					{
						// Create estate object
						estate = m_atlanticCore->newEstate(estateId);
						m_estates[estateId] = estate;

						QObject::connect(estate, SIGNAL(estateToggleMortgage(Estate *)), this, SLOT(estateToggleMortgage(Estate *)));
						QObject::connect(estate, SIGNAL(estateHouseBuy(Estate *)), this, SLOT(estateHouseBuy(Estate *)));
						QObject::connect(estate, SIGNAL(estateHouseSell(Estate *)), this, SLOT(estateHouseSell(Estate *)));
						QObject::connect(estate, SIGNAL(newTrade(Player *)), this, SLOT(newTrade(Player *)));

						b_newEstate = true;
					}

					a = e.attributeNode(QString("name"));
					if (estate && !a.isNull())
						estate->setName(a.value());

					a = e.attributeNode(QString("color"));
					if (estate && !a.isNull() && !a.value().isEmpty())
						estate->setColor(a.value());

					a = e.attributeNode(QString("bgcolor"));
					if (estate && !a.isNull())
						estate->setBgColor(a.value());

					a = e.attributeNode(QString("owner"));
					Player *player;
					if (estate && !a.isNull() && (player = m_players[a.value().toInt()]))
						estate->setOwner(player);

					a = e.attributeNode(QString("houses"));
					if (estate && !a.isNull())
						estate->setHouses(a.value().toInt());

					a = e.attributeNode(QString("mortgaged"));
					if (estate && !a.isNull())
						estate->setIsMortgaged(a.value().toInt());

					a = e.attributeNode(QString("groupid"));
					if (estate && !a.isNull())
						estate->setGroupId(a.value().toInt());

					a = e.attributeNode(QString("can_toggle_mortgage"));
					if (estate && !a.isNull())
						estate->setCanToggleMortgage(a.value().toInt());

					a = e.attributeNode(QString("can_be_owned"));
					if (estate && !a.isNull())
						estate->setCanBeOwned(a.value().toInt());

					a = e.attributeNode(QString("can_buy_houses"));
					if (estate && !a.isNull())
						estate->setCanBuyHouses(a.value().toInt());

					a = e.attributeNode(QString("can_sell_houses"));
					if (estate && !a.isNull())
						estate->setCanSellHouses(a.value().toInt());

					// Emit signal so GUI implementations can create view(s)
#warning port to atlanticcore, but somehow dont create view until all properties are set
					if (b_newEstate)
						emit newEstate(estate);

					if (estate)
						estate->update();
				}
			}
			else if (e.tagName() == "tradeupdate")
			{
				a = e.attributeNode(QString("tradeid"));
				if (!a.isNull())
				{
					int tradeId = a.value().toInt();

					Trade *trade;
					bool b_newTrade = false;
					if (!(trade = m_trades[tradeId]))
					{
						// Create trade object
						trade = m_atlanticCore->newTrade(tradeId);
						m_trades[tradeId] = trade;

						QObject::connect(trade, SIGNAL(updateEstate(Trade *, Estate *, Player *)), this, SLOT(tradeUpdateEstate(Trade *, Estate *, Player *)));
						QObject::connect(trade, SIGNAL(updateMoney(Trade *, unsigned int, Player *, Player *)), this, SLOT(tradeUpdateMoney(Trade *, unsigned int, Player *, Player *)));
						QObject::connect(trade, SIGNAL(reject(Trade *)), this, SLOT(tradeReject(Trade *)));
						b_newTrade = true;
					}

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
								Player *player = m_players[e_player.attributeNode(QString("playerid")).value().toInt()];
								if (trade && player)
									trade->addPlayer(player);
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
									a = e_child.attributeNode(QString("playerid"));
									if (!a.isNull())
									{
										int playerId = a.value().toInt();

										a = e_child.attributeNode(QString("accept"));
										if (!a.isNull())
											emit msgTradeUpdatePlayerAccept(trade->tradeId(), playerId, (bool)(a.value().toInt()));
									}
								}
								else if (e_child.tagName() == "tradeestate")
								{
									kdDebug() << "tradeestate" << endl;
									a = e_child.attributeNode(QString("estateid"));
									if (!a.isNull())
									{
										Estate *estate = m_estates[a.value().toInt()];
										a = e_child.attributeNode(QString("targetplayer"));
										if (!a.isNull())
										{
											Player *player = m_players[a.value().toInt()];
											// Allow NULL player, it will remove the component
											if (trade && estate)
												trade->updateEstate(estate, player);
										}
									}
								}
								else if (e_child.tagName() == "trademoney")
								{
									Player *pFrom = 0, *pTo = 0;

									a = e_child.attributeNode(QString("playerfrom"));
									if (!a.isNull())
										pFrom = m_players[a.value().toInt()];

									a = e_child.attributeNode(QString("playerto"));
									if (!a.isNull())
										pTo = m_players[a.value().toInt()];

									a = e_child.attributeNode(QString("money"));
									if (trade && pFrom && pTo && !a.isNull())
										trade->updateMoney(a.value().toInt(), pFrom, pTo);
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

					// Emit signal so GUI implementations can create view(s)
#warning port to atlanticcore, but somehow dont create view until all properties are set
					if (b_newTrade)
						emit newTrade(trade);

					if (trade)
						trade->update();
				}
			}
			else if (e.tagName() == "auctionupdate")
			{
				a = e.attributeNode(QString("auctionid"));
				if (!a.isNull())
				{
					int auctionId = a.value().toInt();

					Auction *auction;
					bool b_newAuction = false;
					if (!(auction = m_auctions[auctionId]))
					{
						// Create auction object
						auction = m_atlanticCore->newAuction(auctionId, m_estates[e.attributeNode(QString("estateid")).value().toInt()]);
						m_auctions[auctionId] = auction;

						QObject::connect(auction, SIGNAL(bid(Auction *, int)), this, SLOT(auctionBid(Auction *, int)));

						b_newAuction = true;
					}

					QString type = e.attributeNode(QString("type")).value();
					if (type=="new")
					{
					}
					else if (type=="edit")
					{
						a = e.attributeNode(QString("highbidder"));
						if (!a.isNull())
						{
							Player *player = m_players[e.attributeNode(QString("highbidder")).value().toInt()];
							a = e.attributeNode(QString("highbid"));
							if (auction && !a.isNull())
								auction->newBid(player, a.value().toInt());
						}

						a = e.attributeNode(QString("status"));
						if (auction && !a.isNull())
							auction->setStatus(a.value().toInt());

					}
					else if (type=="completed")
						m_atlanticCore->delAuction(auction);

					// Emit signal so GUI implementations can create view(s)
#warning port to atlanticcore, but somehow dont create view until all properties are set
					if (b_newAuction)
						emit newAuction(auction);

					if (auction)
						auction->update();
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

void AtlantikNetwork::serverConnect(const QString host, int port)
{
#ifndef USE_KDE
	connectToHost(host, port);
#else
	setAddress(host, port);
	connect();
#endif
}
