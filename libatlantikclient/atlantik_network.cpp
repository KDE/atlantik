// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <iostream>

#include <qtimer.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <estategroup.h>
#include <trade.h>
#include <auction.h>

#include "atlantik_network.h"
#include "atlantik_network.moc"

//#include "atlantik.h"
//#include "trade_widget.h"

AtlantikNetwork::AtlantikNetwork(AtlanticCore *atlanticCore, QObject *parent, const char *name) : KExtendedSocket(0, 0, KExtendedSocket::inputBufferedSocket)
{
	m_atlanticCore = atlanticCore;
	m_parent = parent;
	m_clientId = m_playerId = -1;

	QObject::connect(this, SIGNAL(readyRead()), this, SLOT(slotRead()));
}

void AtlantikNetwork::rollDice()
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

void AtlantikNetwork::leaveGame()
{
	writeData(".gx");
}

void AtlantikNetwork::endTurn()
{	writeData(".E");
}

void AtlantikNetwork::setName(QString name)
{
	// Almost deprecated, will be replaced by libmonopdprotocol
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

void AtlantikNetwork::getPlayerList()
{	writeData(".gp");
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
	msg.append(QString::number(player ? m_playerRevMap[player] : -1));
	writeData(msg);
}

void AtlantikNetwork::tradeUpdateEstate(Trade *trade, Estate *estate, Player *player)
{
	QString msg(".Te");
	msg.append(QString::number(trade ? trade->tradeId() : -1));
	msg.append(":");
	msg.append(QString::number(estate ? estate->estateId() : -1));
	msg.append(":");
	msg.append(QString::number(player ? m_playerRevMap[player] : -1));
	writeData(msg);
}

void AtlantikNetwork::tradeUpdateMoney(Trade *trade, unsigned int money, Player *pFrom, Player *pTo)
{
	QString msg(".Tm");
	msg.append(QString::number(trade ? trade->tradeId() : -1));
	msg.append(":");
	msg.append(QString::number(pFrom ? m_playerRevMap[pFrom] : -1));
	msg.append(":");
	msg.append(QString::number(pTo ? m_playerRevMap[pTo] : -1));
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

void AtlantikNetwork::tradeAccept(Trade *trade)
{
	QString msg(".Ta");
	msg.append(QString::number(trade ? trade->tradeId() : -1));
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
	if (socketStatus() == KExtendedSocket::connected)
		writeBlock(msg.latin1(), strlen(msg.latin1()));
	else
		kdDebug() << "could not send [" << msg << "]" << std::endl;
}

void AtlantikNetwork::slotRead()
{
	if (canReadLine())
	{
		char *tmp = new char[1024 * 32];
		readLine(tmp, 1024 * 32);
		processMsg(tmp);
		delete[] tmp;

		// There might be more data
		QTimer::singleShot(0, this, SLOT(slotRead()));
	}
	else
	{
		// Maximum message size. Messages won't get bigger than 32k anyway, so
		// if we didn't receive a newline by now, we probably won't anyway.
		if (bytesAvailable() > (1024 * 32))
			flush();
	}
}

void AtlantikNetwork::processMsg(QString str)
{
	kdDebug() << "processing msg: " + str << std::endl;
	msg.setContent(str);
	QDomElement e = msg.documentElement();
	if (e.tagName() != "monopd")
	{
		KMessageBox::detailedError(0, i18n("Atlantik received an invalid response from the monopd server you are connected to. This might cause unexpected behavior."), i18n("The following XML data does not conform to the monopd protocol:\n\n%1").arg(str), i18n("Invalid Response From Server"));
		// Request full update from server
		writeData(".f");
		return;
	}
	QDomNode n = e.firstChild();
	processNode(n);
}

void AtlantikNetwork::processNode(QDomNode n)
{
	QDomAttr a;

	for ( ; !n.isNull() ; n = n.nextSibling() )
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
				}
			}
			else if (e.tagName() == "display")
			{
				QString caption = e.attributeNode(QString("name")).value();
				QString description = e.attributeNode(QString("description")).value();
				
				if (caption.isEmpty() && description.isEmpty())
					emit displayDefault();
				else
				{
					emit displayText(caption, description);
					for( QDomNode nButtons = n.firstChild() ; !nButtons.isNull() ; nButtons = nButtons.nextSibling() )
					{
						QDomElement eButton = nButtons.toElement();
						if (!eButton.isNull() && eButton.tagName() == "button")
							emit displayButton(eButton.attributeNode(QString("command")).value(), eButton.attributeNode(QString("caption")).value());
					}
				}
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
							emit gameListEdit(e_game.attributeNode(QString("id")).value(), e_game.attributeNode(QString("name")).value(), e_game.attributeNode(QString("description")).value(), e_game.attributeNode(QString("players")).value(), e_game.attributeNode(QString("gametype")).value());
						else if (type=="add" || type=="full")
							emit gameListAdd(e_game.attributeNode(QString("id")).value(), e_game.attributeNode(QString("name")).value(), e_game.attributeNode(QString("description")).value(), e_game.attributeNode(QString("players")).value(), e_game.attributeNode(QString("gametype")).value());
					}
					n_game = n_game.nextSibling();
				}
				emit gamelistEndUpdate(type);
			}
			else if (e.tagName() == "joinedgame")
			{
				emit joinedGame();
				getPlayerList();
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
				{
					m_clientId = a.value().toInt();
					// TODO: move to Atlantik::slotNetworkConnected(), not needed on auto-join
					cmdGamesList();
				}
			}
			else if (e.tagName() == "newturn")
			{
				Player *player = m_playerMap[e.attributeNode(QString("player")).value().toInt()];
				if (player)
					// Update *all* objects
					m_atlanticCore->setCurrentTurn(player);
			}
			else if (e.tagName() == "configupdate")
			{
				int gameId = -1;

				a = e.attributeNode(QString("gameid"));
				if (!a.isNull())
				{
					gameId = a.value().toInt();
					for( QDomNode nOptions = n.firstChild() ; !nOptions.isNull() ; nOptions = nOptions.nextSibling() )
					{
						QDomElement eOption = nOptions.toElement();
						if (!eOption.isNull() && eOption.tagName() == "option")
							emit gameOption(eOption.attributeNode(QString("title")).value(), eOption.attributeNode(QString("type")).value(), eOption.attributeNode(QString("value")).value());
					}
				}
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
					else if (status == "start")
						emit gameStarted();
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
					if (!(player = m_playerMap[playerId]))
					{
						// Create player object
						player = m_atlanticCore->newPlayer();
						m_playerMap[playerId] = player;
						m_playerRevMap[player] = playerId;

						if (playerId == m_playerId)
							player->setIsSelf(true);

						b_newPlayer = true;
					}

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
					{
						player->setInJail(a.value().toInt());
						// TODO: emit signal with player ptr so board can setText and display something
					}

					// Update player location
					a = e.attributeNode(QString("location"));
					if (!a.isNull())
					{
						m_playerLocationMap[player] = a.value().toInt();

						Estate *estate = m_estates[a.value().toInt()];
						bool directMove = false;

						a = e.attributeNode(QString("directmove"));
						if (!a.isNull())
							directMove = a.value().toInt();

						if (estate)
							player->setLocation(estate);
					}

					// Emit signal so GUI implementations can create view(s)
#warning port to atlanticcore and create view there
					if (b_newPlayer)
						emit newPlayer(player);

					if (player)
						player->update();
				}
			}
			else if (e.tagName() == "estategroupupdate")
			{
				a = e.attributeNode(QString("name"));
				if (!a.isNull())
				{
					EstateGroup *estateGroup;
					bool b_newEstateGroup = false;
					
					if (!(estateGroup = m_estateGroups[a.value()]))
					{
						// Create EstateGroup object
						estateGroup = m_atlanticCore->newEstateGroup(a.value());
						m_estateGroups[a.value()] = estateGroup;

						b_newEstateGroup = true;
					}

					// Emit signal so GUI implementations can create view(s)
#warning port to atlanticcore and create view there
					if (b_newEstateGroup)
						emit newEstateGroup(estateGroup);

					if (estateGroup)
						estateGroup->update();
				}
			}
			else if (e.tagName() == "estateupdate")
			{
				int estateId = -1;

				a = e.attributeNode(QString("estateid"));
				if (!a.isNull())
				{
					estateId = a.value().toInt();

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

						// Players without estate should get one
						Player *player = 0;
						QPtrList<Player> playerList = m_atlanticCore->players();
						for (QPtrListIterator<Player> it(playerList); (player = *it) ; ++it)
							if (m_playerLocationMap[player] == estate->estateId())
								player->setLocation(estate);
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
					Player *player = m_playerMap[a.value().toInt()];
					if (estate && !a.isNull())
						estate->setOwner(player);

					a = e.attributeNode(QString("houses"));
					if (estate && !a.isNull())
						estate->setHouses(a.value().toInt());

					a = e.attributeNode(QString("mortgaged"));
					if (estate && !a.isNull())
						estate->setIsMortgaged(a.value().toInt());

					a = e.attributeNode(QString("group"));
					if (!a.isNull())
					{
						EstateGroup *estateGroup = m_estateGroups[a.value()];
						if (estate)
							estate->setEstateGroup(estateGroup);
					}

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

					a = e.attributeNode(QString("price"));
					if (estate && !a.isNull())
						estate->setPrice(a.value().toInt());

					// Emit signal so GUI implementations can create view(s)
#warning port to atlanticcore and create view there
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
						QObject::connect(trade, SIGNAL(accept(Trade *)), this, SLOT(tradeAccept(Trade *)));
						b_newTrade = true;
					}

					QString type = e.attributeNode(QString("type")).value();
					if (type=="new")
					{
						// TODO: trade->setActor
						// Player *player = m_playerMap[e.attributeNode(QString("actor")).value().toInt()];
						// if (trade && player)
						// 	trade->setActor(player);

						QDomNode n_player = n.firstChild();
						while(!n_player.isNull())
						{
							QDomElement e_player = n_player.toElement();
							if (!e_player.isNull() && e_player.tagName() == "tradeplayer")
							{
								Player *player = m_playerMap[e_player.attributeNode(QString("playerid")).value().toInt()];
								if (trade && player)
									trade->addPlayer(player);
							}
							n_player = n_player.nextSibling();
						}
					}
					else if (type=="accepted" && trade)
					{
						m_atlanticCore->delTrade(trade);
						trade = 0;
					}
					else if (type=="completed" && trade)
						emit msgTradeUpdateCompleted(trade);
					else if (type=="rejected")
					{
						Player *player = m_playerMap[e.attributeNode(QString("actor")).value().toInt()];
						if (trade)
							trade->reject(player);
						if (player && player->isSelf())
						{
							m_atlanticCore->delTrade(trade);
							m_trades[tradeId] = 0;
							trade = 0;
						}
					}
					else
					{
						// No type specified, edit is implied.

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
										Player *player = m_playerMap[a.value().toInt()];

										a = e_child.attributeNode(QString("accept"));
										if (trade && player && !a.isNull())
											trade->updateAccept(player, (bool)(a.value().toInt()));
									}
								}
								else if (e_child.tagName() == "tradeestate")
								{
									a = e_child.attributeNode(QString("estateid"));
									if (!a.isNull())
									{
										Estate *estate = m_estates[a.value().toInt()];
										a = e_child.attributeNode(QString("targetplayer"));
										if (!a.isNull())
										{
											Player *player = m_playerMap[a.value().toInt()];
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
										pFrom = m_playerMap[a.value().toInt()];

									a = e_child.attributeNode(QString("playerto"));
									if (!a.isNull())
										pTo = m_playerMap[a.value().toInt()];

									a = e_child.attributeNode(QString("money"));
									kdDebug() << "tradeupdatemoney" << (pFrom ? "1" : "0") << (pTo ? "1" : "0") << (a.isNull() ? "0" : "1") << std::endl;
									if (trade && pFrom && pTo && !a.isNull())
										trade->updateMoney(a.value().toInt(), pFrom, pTo);
								}
							}
							n_child = n_child.nextSibling();
						}
					}

					// Emit signal so GUI implementations can create view(s)
#warning port to atlanticcore and create view there
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

					a = e.attributeNode(QString("highbidder"));
					if (!a.isNull())
					{
						Player *player = m_playerMap[e.attributeNode(QString("highbidder")).value().toInt()];
						a = e.attributeNode(QString("highbid"));
						if (auction && !a.isNull())
							auction->newBid(player, a.value().toInt());
					}

					a = e.attributeNode(QString("status"));
					if (auction && !a.isNull())
					{
						int status = a.value().toInt();
						auction->setStatus(status);

						// TODO: find a good way to visualise "sold!"
						if (status == 3)
						{
							m_atlanticCore->delAuction(auction);
							m_auctions[auctionId] = 0;
							auction = 0;
						}
					}

					// Emit signal so GUI implementations can create view(s)
#warning port to atlanticcore and create view there
					if (b_newAuction)
						emit newAuction(auction);

					if (auction)
						auction->update();
				}
			}
			else
				kdDebug() << "ignored TAG: " << e.tagName() << std::endl;
		}
		// TODO: remove permanently?
		// QDomNode node = n.firstChild();
		// processNode(node);
	}
}

void AtlantikNetwork::serverConnect(const QString host, int port)
{
	setAddress(host, port);
	enableRead(true);
	startAsyncConnect();
}
