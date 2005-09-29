// Copyright (c) 2002-2004 Rob Kaper <cap@capsi.com>
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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include <iostream>

#include <qdom.h>
#include <qtextcodec.h>
#include <qtextstream.h>
#include <qtimer.h>

#include <kdebug.h>
#include <klocale.h>

#include <atlantic_core.h>
#include <auction.h>
#include <configoption.h>
#include <estate.h>
#include <estategroup.h>
#include <game.h>
#include <player.h>
#include <trade.h>

#include "atlantik_network.h"

AtlantikNetwork::AtlantikNetwork(AtlanticCore *atlanticCore) : KExtendedSocket(0, 0, KExtendedSocket::inputBufferedSocket)
{
	m_atlanticCore = atlanticCore;
	m_textStream = new QTextStream(this);
	m_textStream->setCodec(QTextCodec::codecForName("utf8"));
	m_playerId = -1;
	m_serverVersion = "";

	QObject::connect(this, SIGNAL(readyRead()), this, SLOT(slotRead()));
	QObject::connect(this, SIGNAL(lookupFinished(int)),
	                 this, SLOT(slotLookupFinished(int)));
	QObject::connect(this, SIGNAL(connectionSuccess()),
	                 this, SLOT(slotConnectionSuccess()));
	QObject::connect(this, SIGNAL(connectionFailed(int)),
	                 this, SLOT(slotConnectionFailed(int)));
}

AtlantikNetwork::~AtlantikNetwork(void)
{
	delete m_textStream;
}

void AtlantikNetwork::rollDice()
{
	writeData(".r");
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

void AtlantikNetwork::reconnect(const QString &cookie)
{
	writeData(".R" + cookie);
}

void AtlantikNetwork::leaveGame()
{
	writeData(".gx");
}

void AtlantikNetwork::endTurn()
{
	writeData(".E");
}

void AtlantikNetwork::setName(QString name)
{
	// Almost deprecated, will be replaced by libmonopdprotocol
	writeData(QString(".n%1").arg(name));
}

void AtlantikNetwork::tokenConfirmation(Estate *estate)
{
	writeData(QString(".t%1").arg(estate ? estate->id() : -1));
}

void AtlantikNetwork::estateToggleMortgage(Estate *estate)
{
	writeData(QString(".em%1").arg(estate ? estate->id() : -1));
}

void AtlantikNetwork::estateHouseBuy(Estate *estate)
{
	writeData(QString(".hb%1").arg(estate ? estate->id() : -1));
}

void AtlantikNetwork::estateHouseSell(Estate *estate)
{
	writeData(QString(".hs%1").arg(estate ? estate->id() : -1));
}

void AtlantikNetwork::newGame(const QString &gameType)
{
	writeData(QString(".gn%1").arg(gameType));
}

void AtlantikNetwork::joinGame(int gameId)
{
	writeData(QString(".gj%1").arg(gameId));
}

void AtlantikNetwork::cmdChat(QString msg)
{
	writeData(msg);
}

void AtlantikNetwork::newTrade(Player *player)
{
	writeData(QString(".Tn%1").arg(player ? player->id() : -1));
}

void AtlantikNetwork::kickPlayer(Player *player)
{
	writeData(QString(".gk%1").arg(player ? player->id() : -1));
}

void AtlantikNetwork::tradeUpdateEstate(Trade *trade, Estate *estate, Player *player)
{
	writeData(QString(".Te%1:%2:%3").arg(trade ? trade->tradeId() : -1).arg(estate ? estate->id() : -1).arg(player ? player->id() : -1));
}

void AtlantikNetwork::tradeUpdateMoney(Trade *trade, unsigned int money, Player *pFrom, Player *pTo)
{
	writeData(QString(".Tm%1:%2:%3:%4").arg(trade ? trade->tradeId() : -1).arg(pFrom ? pFrom->id() : -1).arg(pTo ? pTo->id() : -1).arg(money));
}

void AtlantikNetwork::tradeReject(Trade *trade)
{
	writeData(QString(".Tr%1").arg(trade ? trade->tradeId() : -1));
}

void AtlantikNetwork::tradeAccept(Trade *trade)
{
	writeData(QString(".Ta%1:%2").arg(trade ? trade->tradeId() : -1).arg(trade ? trade->revision() : -1));
}

void AtlantikNetwork::auctionBid(Auction *auction, int amount)
{
	writeData(QString(".ab%1:%2").arg(auction ? auction->auctionId() : -1).arg(amount));
}

void AtlantikNetwork::setImage(const QString &name)
{
	writeData(QString(".pi%1").arg(name));
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

void AtlantikNetwork::changeOption(int configId, const QString &value)
{
	writeData( QString(".gc%1:%2").arg(configId).arg(value) );
}

void AtlantikNetwork::writeData(QString msg)
{
	emit networkEvent(msg, "1rightarrow");
	msg.append("\n");
	if (socketStatus() == KExtendedSocket::connected)
		*m_textStream << msg;
	else
		kdDebug() << "warning: socket not connected!" << endl;
}

void AtlantikNetwork::slotRead()
{
	if ( socketStatus() != KExtendedSocket::connected )
		return;

	if (canReadLine())
	{
		processMsg(m_textStream->readLine());
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

void AtlantikNetwork::processMsg(const QString &msg)
{
	emit networkEvent(msg, "1leftarrow");

	QDomDocument dom;
	dom.setContent(msg);
	QDomElement e = dom.documentElement();
	if (e.tagName() != "monopd")
	{
		// Invalid data, request full update from server
		writeData(".f");
		return;
	}
	QDomNode n = e.firstChild();
	processNode(n);
	m_atlanticCore->printDebug();
}

void AtlantikNetwork::processNode(QDomNode n)
{
	QDomAttr a;

	for ( ; !n.isNull() ; n = n.nextSibling() )
	{
		QDomElement e = n.toElement();
		if(!e.isNull())
		{
			if (e.tagName() == "server")
			{
				a = e.attributeNode( QString("version") );
				if ( !a.isNull() )
					m_serverVersion = a.value();

				emit receivedHandshake();
			}
			else if (e.tagName() == "msg")
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
				int estateId = -1;

				a = e.attributeNode(QString("estateid"));
				if (!a.isNull())
				{
					estateId = a.value().toInt();
					Estate *estate;
					estate = m_atlanticCore->findEstate(a.value().toInt());

					emit displayDetails(e.attributeNode(QString("text")).value(), e.attributeNode(QString("cleartext")).value().toInt(), e.attributeNode(QString("clearbuttons")).value().toInt(), estate);

					bool hasButtons = false;
					for( QDomNode nButtons = n.firstChild() ; !nButtons.isNull() ; nButtons = nButtons.nextSibling() )
					{
						QDomElement eButton = nButtons.toElement();
						if (!eButton.isNull() && eButton.tagName() == "button")
						{
							emit addCommandButton(eButton.attributeNode(QString("command")).value(), eButton.attributeNode(QString("caption")).value(), eButton.attributeNode(QString("enabled")).value().toInt());
							hasButtons = true;
						}
					}

					if (!hasButtons)
						emit addCloseButton();
				}
			}
			else if (e.tagName() == "client")
			{
				a = e.attributeNode(QString("playerid"));
				if (!a.isNull())
					m_playerId = a.value().toInt();

				a = e.attributeNode(QString("cookie"));
				if (!a.isNull())
					emit clientCookie(a.value());
			}
			else if (e.tagName() == "configupdate")
			{
				int configId = -1;
				a = e.attributeNode(QString("configid"));
				if (!a.isNull())
				{
					configId = a.value().toInt();
					ConfigOption *configOption;
					if (!(configOption = m_atlanticCore->findConfigOption(configId)))
						configOption = m_atlanticCore->newConfigOption( configId );

					a = e.attributeNode(QString("name"));
					if (configOption && !a.isNull())
						configOption->setName(a.value());

					a = e.attributeNode(QString("description"));
					if (configOption && !a.isNull())
						configOption->setDescription(a.value());

					a = e.attributeNode(QString("edit"));
					if (configOption && !a.isNull())
						configOption->setEdit(a.value().toInt());

					a = e.attributeNode(QString("value"));
					if (configOption && !a.isNull())
						configOption->setValue(a.value());

					if (configOption)
						configOption->update();
				}

				int gameId = -1;
				a = e.attributeNode(QString("gameid"));
				if (!a.isNull())
				{
					gameId = a.value().toInt();
					for( QDomNode nOptions = n.firstChild() ; !nOptions.isNull() ; nOptions = nOptions.nextSibling() )
					{
						QDomElement eOption = nOptions.toElement();
						if (!eOption.isNull() && eOption.tagName() == "option")
							emit gameOption(eOption.attributeNode(QString("title")).value(), eOption.attributeNode(QString("type")).value(), eOption.attributeNode(QString("value")).value(), eOption.attributeNode(QString("edit")).value(), eOption.attributeNode(QString("command")).value());
					}
					emit endConfigUpdate();
				}
			}
			else if (e.tagName() == "deletegame")
			{
				a = e.attributeNode(QString("gameid"));
				if (!a.isNull())
				{
					int gameId = a.value().toInt();

					Game *game = m_atlanticCore->findGame(gameId);
					if (game)
						m_atlanticCore->removeGame(game);
				}
			}
			else if (e.tagName() == "gameupdate")
			{
				int gameId = -1;

				a = e.attributeNode(QString("gameid"));
				if (!a.isNull())
				{
					gameId = a.value().toInt();

					Player *playerSelf = m_atlanticCore->playerSelf();
					if ( playerSelf && playerSelf->game() )
						kdDebug() << "gameupdate for " << QString::number(gameId) << " with playerSelf in game " << QString::number(playerSelf->game()->id()) << endl;
					else
						kdDebug() << "gameupdate for " << QString::number(gameId) << endl;


					Game *game = 0;
					if (gameId == -1)
					{
						a = e.attributeNode(QString("gametype"));
						if ( !a.isNull() && !(game = m_atlanticCore->findGame(a.value())) )
							game = m_atlanticCore->newGame(gameId, a.value());
					}
					else if (!(game = m_atlanticCore->findGame(gameId)))
						game = m_atlanticCore->newGame(gameId);

					a = e.attributeNode(QString("canbejoined"));
					if (game && !a.isNull())
						game->setCanBeJoined(a.value().toInt());

					a = e.attributeNode(QString("description"));
					if (game && !a.isNull())
						game->setDescription(a.value());

					a = e.attributeNode(QString("name"));
					if (game && !a.isNull())
						game->setName(a.value());

					a = e.attributeNode(QString("players"));
					if (game && !a.isNull())
						game->setPlayers(a.value().toInt());

					a = e.attributeNode(QString("master"));
					if (game && !a.isNull())
					{
						// Ensure setMaster succeeds by creating player if necessary
						Player *player = m_atlanticCore->findPlayer( a.value().toInt() );
						if ( !player )
							player = m_atlanticCore->newPlayer( a.value().toInt() );
						game->setMaster( player );
					}

					QString status = e.attributeNode(QString("status")).value();
					if ( m_serverVersion.left(4) == "0.9." || (playerSelf && playerSelf->game() == game) )
					{
						if (status == "config")
							emit gameConfig();
						else if (status == "init")
							emit gameInit();
						else if (status == "run")
							emit gameRun();
						else if (status == "end")
							emit gameEnd();
					}

					if (game)
						game->update();
				}
			}
			else if (e.tagName() == "deleteplayer")
			{
				a = e.attributeNode(QString("playerid"));
				if (!a.isNull())
				{
					int playerId = a.value().toInt();

					Player *player = m_atlanticCore->findPlayer(playerId);
					if (player)
						m_atlanticCore->removePlayer(player);
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
					if (!(player = m_atlanticCore->findPlayer(playerId)))
						player = m_atlanticCore->newPlayer( playerId, (m_playerId == playerId) );

					// Update player name
					a = e.attributeNode(QString("name"));
					if (player && !a.isNull())
						player->setName(a.value());

					// Update player game
					a = e.attributeNode(QString("game"));
					if (player && !a.isNull())
					{
						int gameId = a.value().toInt();
						if (gameId == -1)
							player->setGame( 0 );
						else
						{
							// Ensure setGame succeeds by creating game if necessary
							Game *game = m_atlanticCore->findGame(a.value().toInt());
							if (!game)
								game = m_atlanticCore->newGame(a.value().toInt()); // 
							player->setGame( game );
						}
					}

					// Update player host
					a = e.attributeNode(QString("host"));
					if (player && !a.isNull())
						player->setHost(a.value());

					// Update player image/token
					a = e.attributeNode(QString("image"));
					if (player && !a.isNull())
						player->setImage(a.value());

					// Update player money
					a = e.attributeNode(QString("money"));
					if (player && !a.isNull())
						player->setMoney(a.value().toInt());

					a = e.attributeNode(QString("bankrupt"));
					if (player && !a.isNull())
						player->setBankrupt(a.value().toInt());

					a = e.attributeNode(QString("hasdebt"));
					if (player && !a.isNull())
						player->setHasDebt(a.value().toInt());

					a = e.attributeNode(QString("hasturn"));
					if (player && !a.isNull())
						player->setHasTurn(a.value().toInt());

					// Update whether player can roll
					a = e.attributeNode(QString("can_roll"));
					if (player && !a.isNull())
						player->setCanRoll(a.value().toInt());

					// Update whether player can buy
					a = e.attributeNode(QString("can_buyestate"));
					if (player && !a.isNull())
						player->setCanBuy(a.value().toInt());

					// Update whether player can auction
					a = e.attributeNode(QString("canauction"));
					if (player && !a.isNull())
						player->setCanAuction(a.value().toInt());

					// Update whether player can use a card
					a = e.attributeNode(QString("canusecard"));
					if (player && !a.isNull())
						player->setCanUseCard(a.value().toInt());

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

						bool directMove = false;

						Estate *estate = m_atlanticCore->findEstate(a.value().toInt());

						a = e.attributeNode(QString("directmove"));
						if (!a.isNull())
							directMove = a.value().toInt();

						if (player && estate)
						{
							if (directMove)
								player->setLocation(estate);
							else
								player->setDestination(estate);
						}
					}

					if (player)
						player->update();
				}
			}
			else if (e.tagName() == "estategroupupdate")
			{
				a = e.attributeNode(QString("groupid"));
				if (!a.isNull())
				{
					int groupId = a.value().toInt();

					EstateGroup *estateGroup = 0;
					bool b_newEstateGroup = false;
					
					if (!(estateGroup = m_atlanticCore->findEstateGroup(groupId)))
					{
						// Create EstateGroup object
						estateGroup = m_atlanticCore->newEstateGroup(a.value().toInt());
						b_newEstateGroup = true;
					}

					a = e.attributeNode(QString("name"));
					if (estateGroup && !a.isNull())
						estateGroup->setName(a.value());

					// Emit signal so GUI implementations can create view(s)
					// TODO:  port to atlanticcore and create view there
					if (estateGroup)
					{
						if (b_newEstateGroup)
							emit newEstateGroup(estateGroup);
						estateGroup->update();
					}
				}
			}
			else if (e.tagName() == "estateupdate")
			{
				int estateId = -1;

				a = e.attributeNode(QString("estateid"));
				if (!a.isNull())
				{
					estateId = a.value().toInt();
					
					Estate *estate = 0;
					bool b_newEstate = false;

					// FIXME: allow any estateId, GUI should not use it to determin its geometry
					if (estateId >= 0 && estateId < 100 && !(estate = m_atlanticCore->findEstate(a.value().toInt())))
					{
						// Create estate object
						estate = m_atlanticCore->newEstate(estateId);
						b_newEstate = true;

						QObject::connect(estate, SIGNAL(estateToggleMortgage(Estate *)), this, SLOT(estateToggleMortgage(Estate *)));
						QObject::connect(estate, SIGNAL(estateHouseBuy(Estate *)), this, SLOT(estateHouseBuy(Estate *)));
						QObject::connect(estate, SIGNAL(estateHouseSell(Estate *)), this, SLOT(estateHouseSell(Estate *)));
						QObject::connect(estate, SIGNAL(newTrade(Player *)), this, SLOT(newTrade(Player *)));

						// Players without estate should get one
						Player *player = 0;
						QPtrList<Player> playerList = m_atlanticCore->players();
						for (QPtrListIterator<Player> it(playerList); (player = *it) ; ++it)
							if (m_playerLocationMap[player] == estate->id())
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
					Player *player = m_atlanticCore->findPlayer(a.value().toInt());
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
						EstateGroup *estateGroup = m_atlanticCore->findEstateGroup(a.value().toInt());
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

					a = e.attributeNode(QString("houseprice"));
        				if (estate && !a.isNull())
                				estate->setHousePrice(a.value().toInt());

        				a = e.attributeNode(QString("sellhouseprice"));
        				if (estate && !a.isNull())
                				estate->setHouseSellPrice(a.value().toInt());
						
					a = e.attributeNode(QString("mortgageprice"));
        				if (estate && !a.isNull())
                				estate->setMortgagePrice(a.value().toInt());

        				a = e.attributeNode(QString("unmortgageprice"));
        				if (estate && !a.isNull())
                				estate->setUnmortgagePrice(a.value().toInt());
						
					a = e.attributeNode(QString("money"));
					if (estate && !a.isNull())
						estate->setMoney(a.value().toInt());

					// Emit signal so GUI implementations can create view(s)
					// TODO:  port to atlanticcore and create view there
					if (estate)
					{
						if (b_newEstate)
							emit newEstate(estate);
						estate->update();
					}
				}
			}
			else if (e.tagName() == "tradeupdate")
			{
				a = e.attributeNode(QString("tradeid"));
				if (!a.isNull())
				{
					int tradeId = a.value().toInt();

					Trade *trade = m_atlanticCore->findTrade(tradeId);
					if (!trade)
					{
						// Create trade object
						trade = m_atlanticCore->newTrade(tradeId);

						QObject::connect(trade, SIGNAL(updateEstate(Trade *, Estate *, Player *)), this, SLOT(tradeUpdateEstate(Trade *, Estate *, Player *)));
						QObject::connect(trade, SIGNAL(updateMoney(Trade *, unsigned int, Player *, Player *)), this, SLOT(tradeUpdateMoney(Trade *, unsigned int, Player *, Player *)));
						QObject::connect(trade, SIGNAL(reject(Trade *)), this, SLOT(tradeReject(Trade *)));
						QObject::connect(trade, SIGNAL(accept(Trade *)), this, SLOT(tradeAccept(Trade *)));
					}

					a = e.attributeNode(QString("revision"));
					if (trade && !a.isNull())
						trade->setRevision(a.value().toInt());

					QString type = e.attributeNode(QString("type")).value();
					if (type=="new")
					{
						// TODO: trade->setActor
						// Player *player = m_atlanticCore->findPlayer(e.attributeNode(QString("actor")).value().toInt());
						// if (trade && player)
						// 	trade->setActor(player);

						QDomNode n_player = n.firstChild();
						while(!n_player.isNull())
						{
							QDomElement e_player = n_player.toElement();
							if (!e_player.isNull() && e_player.tagName() == "tradeplayer")
							{
								Player *player = m_atlanticCore->findPlayer(e_player.attributeNode(QString("playerid")).value().toInt());
								if (trade && player)
								{
									trade->addPlayer(player);
									QObject::connect(m_atlanticCore, SIGNAL(removePlayer(Player *)), trade, SLOT(removePlayer(Player *)));
								}
							}
							n_player = n_player.nextSibling();
						}
					}
					else if (type=="accepted" && trade)
						emit msgTradeUpdateAccepted(trade);
					else if (type=="completed" && trade)
					{
						m_atlanticCore->removeTrade(trade);
						trade = 0;
					}
					else if (type=="rejected")
					{
						Player *player = m_atlanticCore->findPlayer(e.attributeNode(QString("actor")).value().toInt());
						if (trade)
							trade->reject(player);
						if ( player && player == m_atlanticCore->playerSelf() )
						{
							m_atlanticCore->removeTrade(trade);
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
										Player *player = m_atlanticCore->findPlayer(a.value().toInt());

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
										Estate *estate = m_atlanticCore->findEstate(a.value().toInt());
										a = e_child.attributeNode(QString("targetplayer"));
										if (!a.isNull())
										{
											Player *player = m_atlanticCore->findPlayer(a.value().toInt());
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
										pFrom = m_atlanticCore->findPlayer(a.value().toInt());

									a = e_child.attributeNode(QString("playerto"));
									if (!a.isNull())
										pTo = m_atlanticCore->findPlayer(a.value().toInt());

									a = e_child.attributeNode(QString("money"));
									kdDebug() << "tradeupdatemoney" << (pFrom ? "1" : "0") << (pTo ? "1" : "0") << (a.isNull() ? "0" : "1") << endl;
									if (trade && pFrom && pTo && !a.isNull())
										trade->updateMoney(a.value().toInt(), pFrom, pTo);
								}
							}
							n_child = n_child.nextSibling();
						}
					}

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
						auction = m_atlanticCore->newAuction(auctionId, m_atlanticCore->findEstate(e.attributeNode(QString("estateid")).value().toInt()));
						m_auctions[auctionId] = auction;

						QObject::connect(auction, SIGNAL(bid(Auction *, int)), this, SLOT(auctionBid(Auction *, int)));

						b_newAuction = true;
					}

					a = e.attributeNode(QString("highbidder"));
					if (!a.isNull())
					{
						Player *player = m_atlanticCore->findPlayer(e.attributeNode(QString("highbidder")).value().toInt());
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
					// TODO:  port to atlanticcore and create view there
					if (auction)
					{
						if (b_newAuction)
							emit newAuction(auction);
						auction->update();
					}
				}
			}
			else
				kdDebug() << "ignored TAG: " << e.tagName() << endl;
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
	emit msgStatus(i18n("Connecting to %1:%2...").arg(host).arg(QString::number(port)), "connect_creating");
	startAsyncConnect();
}

void AtlantikNetwork::slotLookupFinished(int count)
{
	emit msgStatus(i18n("Server host name lookup finished..."));
}

void AtlantikNetwork::slotConnectionSuccess()
{
	emit msgStatus(i18n("Connected to %1:%2.").arg(host()).arg(port()), "connect_established");
}

void AtlantikNetwork::slotConnectionFailed(int error)
{
	emit msgStatus(i18n("Connection failed! Error code: %1").arg(error), "connect_no");
}

#include "atlantik_network.moc"
