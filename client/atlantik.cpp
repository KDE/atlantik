// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <errno.h>

#include <qcolor.h>
#include <qlineedit.h>
#include <qscrollbar.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstdgameaction.h>
#include <kstdaction.h>
#include <ktoolbar.h>
 
#include <atlantic_core.h>
#include <auction.h>
#include <estate.h>
#include <player.h>
#include <trade.h>
#include "atlantik.moc"

#include <atlantik_network.h>

#include <board.h>
#include <trade_widget.h>

#include "selectserver_widget.h"
#include "selectgame_widget.h"
#include "selectconfiguration_widget.h"

Atlantik::Atlantik () : KMainWindow ()
{
	// Read application configuration
	readConfig();

	// Toolbar: Game
//	KStdGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
	KStdGameAction::quit(kapp, SLOT(closeAllWindows()), actionCollection(), "game_quit");

	// Toolbar: Settings
	KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());

	// Initialize pointers to 0L
	m_configDialog = 0;
	m_playerSelf = 0;
	m_board = 0;

	// Game core
	m_atlanticCore = new AtlanticCore(this, "atlanticCore");

	connect(m_atlanticCore, SIGNAL(removeGUI(Trade *)), this, SLOT(removeGUI(Trade *)));

	// Network layer
	m_atlantikNetwork = new AtlantikNetwork(m_atlanticCore, this, "atlantikNetwork");

	connect(m_atlantikNetwork, SIGNAL(msgInfo(QString)), this, SLOT(slotMsgInfo(QString)));
	connect(m_atlantikNetwork, SIGNAL(msgError(QString)), this, SLOT(slotMsgError(QString)));
	connect(m_atlantikNetwork, SIGNAL(msgChat(QString, QString)), this, SLOT(slotMsgChat(QString, QString)));

	connect(m_atlantikNetwork, SIGNAL(connectionSuccess()), this, SLOT(slotNetworkConnected()));
	connect(m_atlantikNetwork, SIGNAL(connectionFailed(int)), this, SLOT(slotNetworkError(int)));

	connect(m_atlantikNetwork, SIGNAL(joinedGame()), this, SLOT(slotJoinedGame()));
	connect(m_atlantikNetwork, SIGNAL(initGame()), this, SLOT(initGame()));
	connect(m_atlantikNetwork, SIGNAL(gameStarted()), this, SLOT(gameStarted()));

	connect(m_atlantikNetwork, SIGNAL(newPlayer(Player *)), this, SLOT(newPlayer(Player *)));
	connect(m_atlantikNetwork, SIGNAL(newEstate(Estate *)), this, SLOT(newEstate(Estate *)));
	connect(m_atlantikNetwork, SIGNAL(newTrade(Trade *)), this, SLOT(createGUI(Trade *)));
	connect(m_atlantikNetwork, SIGNAL(newAuction(Auction *)), this, SLOT(newAuction(Auction *)));

	// Menu,toolbar: Move
	m_roll = KStdGameAction::roll(m_atlantikNetwork, SLOT(rollDice()), actionCollection());
	m_roll->setEnabled(false);
	m_buyEstate = new KAction(i18n("&Buy"), "atlantik_buy_estate", CTRL+Key_B, m_atlantikNetwork, SLOT(buyEstate()), actionCollection(), "buy_estate");
	m_buyEstate->setEnabled(false);
	m_auctionEstate = new KAction(i18n("&Auction"), "auction", CTRL+Key_A, m_atlantikNetwork, SLOT(auctionEstate()), actionCollection(), "auction");
	m_auctionEstate->setEnabled(false);
	m_endTurn = KStdGameAction::endTurn(m_atlantikNetwork, SLOT(endTurn()), actionCollection());
	m_endTurn->setEnabled(false);
	m_jailCard = new KAction(i18n("Use Card to Leave Jail"), "altantik_move_jail_card", 0, m_atlantikNetwork, SLOT(jailCard()), actionCollection(), "move_jailcard");
	m_jailCard->setEnabled(false);
	m_jailPay = new KAction(i18n("&Pay to Leave Jail"), "jail_pay", CTRL+Key_P, m_atlantikNetwork, SLOT(jailPay()), actionCollection(), "move_jailpay");
	m_jailPay->setEnabled(false);
	m_jailRoll = new KAction(i18n("&Roll to Leave Jail"), "altantik_move_jail_roll", CTRL+Key_R, m_atlantikNetwork, SLOT(jailRoll()), actionCollection(), "move_jailroll");
	m_jailRoll->setEnabled(false);

	// Mix code and XML into GUI
	KMainWindow::createGUI();

	// Main widget, containing all others
 	m_mainWidget = new QWidget(this, "main");
	m_mainWidget->show();
	m_mainLayout = new QGridLayout(m_mainWidget, 3, 2);
	setCentralWidget(m_mainWidget);

	// Vertical view area for portfolios.
	m_portfolioWidget = new QWidget(m_mainWidget, "pfwidget");
	m_mainLayout->addWidget(m_portfolioWidget, 0, 0);
	m_portfolioWidget->show();
	m_portfolioLayout = new QVBoxLayout(m_portfolioWidget);

	// Nice label
//	m_portfolioLabel = new QLabel(i18n("Players"), m_portfolioWidget, "pfLabel");
//	m_portfolioLayout->addWidget(m_portfolioLabel);
//	m_portfolioLabel->show();

	// Text view for chat and status messages from server.
	m_serverMsgs = new QTextEdit(m_mainWidget, "serverMsgs");
	m_serverMsgs->setReadOnly(true);
	m_serverMsgs->setHScrollBarMode(QScrollView::AlwaysOff);
	m_serverMsgs->setFixedWidth(225);
	m_mainLayout->addWidget(m_serverMsgs, 1, 0);

	// LineEdit to enter commands and chat messages.
	m_input = new QLineEdit(m_mainWidget, "input");
	m_mainLayout->addWidget(m_input, 2, 0);

	connect(m_input, SIGNAL(returnPressed()), this, SLOT(slotSendMsg()));

	// Set stretching where we want it.
	m_mainLayout->setRowStretch(1, 1); // make m_board+m_serverMsgs stretch vertically, not the rest
	m_mainLayout->setColStretch(1, 1); // make m_board stretch horizontally, not the rest

	m_selectServer = new SelectServer(m_mainWidget, "selectServer");
	m_mainLayout->addMultiCellWidget(m_selectServer, 0, 2, 1, 1);
	m_selectServer->show();

	connect(m_selectServer, SIGNAL(serverConnect(const QString, int)), m_atlantikNetwork, SLOT(serverConnect(const QString, int)));

	m_selectGame = 0;
	m_selectConfiguration = 0;
}

void Atlantik::readConfig()
{
	// Read configuration settings
	KConfig *config = kapp->config();

	// Personalization configuration
	config->setGroup("Personalization");
	m_config.playerName = config->readEntry("PlayerName", "Atlantik");

	// Board configuration
	config->setGroup("Board");
	m_config.indicateUnowned = config->readBoolEntry("IndicateUnowned", true);
	m_config.highliteUnowned = config->readBoolEntry("HighliteUnowned", false);
	m_config.darkenMortgaged = config->readBoolEntry("DarkenMortgaged", true);
	m_config.animateTokens = config->readBoolEntry("AnimateToken", false);
	m_config.quartzEffects = config->readBoolEntry("QuartzEffects", true);

	// Portfolio colors
	config->setGroup("WM");
	QColor activeDefault(204, 204, 204), inactiveDefault(153, 153, 153);
	m_config.activeColor = config->readColorEntry("activeBackground", &activeDefault);
	m_config.inactiveColor = config->readColorEntry("inactiveBlend", &inactiveDefault);
}

void Atlantik::newPlayer(Player *player)
{
	if (!m_board)
		initGame();

	m_board->addToken(player);

	PortfolioView *portfolioView = new PortfolioView(m_atlanticCore, player, m_config.activeColor, m_config.inactiveColor, m_portfolioWidget);
	m_portfolioViews.append(portfolioView);

	if (player->isSelf())
	{
		m_playerSelf = player;
		connect(player, SIGNAL(changed(Player *)), this, SLOT(playerChanged()));
	}
	connect(player, SIGNAL(changed(Player *)), portfolioView, SLOT(playerChanged()));
	connect(portfolioView, SIGNAL(newTrade(Player *)), m_atlantikNetwork, SLOT(newTrade(Player *)));

	m_portfolioLayout->addWidget(portfolioView);
	portfolioView->show();
}

void Atlantik::newEstate(Estate *estate)
{
	if (!m_board)
		initGame();

	m_board->addEstateView(estate, m_config.indicateUnowned, m_config.highliteUnowned, m_config.darkenMortgaged, m_config.quartzEffects);
}

void Atlantik::createGUI(Trade *trade)
{
	TradeDisplay *tradeDisplay = new TradeDisplay(trade, m_atlanticCore, 0, "tradeDisplay");
	m_tradeGUIMap[trade] = tradeDisplay;
	tradeDisplay->show();
}

void Atlantik::newAuction(Auction *auction)
{
	if (!m_board)
		initGame();

	m_board->addAuctionWidget(auction);
}

void Atlantik::removeGUI(Trade *trade)
{
	if (TradeDisplay *tradeDisplay = m_tradeGUIMap[trade])
		delete tradeDisplay;
}

void Atlantik::slotNetworkConnected()
{
	kdDebug() << "Atlantik::slotNetworkConnected()" << endl;

	// We're connected, so let's make ourselves known.
	m_atlantikNetwork->cmdName(m_config.playerName);

	// Create select game widget and replace the select server widget.
	m_selectGame = new SelectGame(m_mainWidget, "selectGame");
	m_mainLayout->addMultiCellWidget(m_selectGame, 0, 2, 1, 1);
	m_selectGame->show();
	if (m_selectServer)
	{
		delete m_selectServer;
		m_selectServer = 0;
	}

	connect(m_atlantikNetwork, SIGNAL(gameListClear()), m_selectGame, SLOT(slotGameListClear()));
	connect(m_atlantikNetwork, SIGNAL(gameListAdd(QString, QString, QString, QString)), m_selectGame, SLOT(slotGameListAdd(QString, QString, QString, QString)));
	connect(m_atlantikNetwork, SIGNAL(gameListEdit(QString, QString, QString, QString)), m_selectGame, SLOT(slotGameListEdit(QString, QString, QString, QString)));
	connect(m_atlantikNetwork, SIGNAL(gameListDel(QString)), m_selectGame, SLOT(slotGameListDel(QString)));

	connect(m_selectGame, SIGNAL(joinGame(int)), m_atlantikNetwork, SLOT(joinGame(int)));
	connect(m_selectGame, SIGNAL(newGame(const QString &)), m_atlantikNetwork, SLOT(newGame(const QString &)));
}

void Atlantik::slotNetworkError(int errnum)
{
	QString errMsg(i18n("Error connecting: "));
	
	switch (m_atlantikNetwork->status())
	{
		case IO_ConnectError:
			if (errnum == ECONNREFUSED)
				errMsg.append(i18n("connection refused by host."));
			else
				errMsg.append(i18n("could not connect to host."));
			break;

		case IO_LookupError:
			errMsg.append(i18n("host not found."));
			break;

		default:
			errMsg.append(i18n("unknown error."));
	}

	serverMsgsAppend(errMsg);
}

void Atlantik::slotJoinedGame()
{
	// Create configuration widget and replace the select game widget.
	m_selectConfiguration = new SelectConfiguration(m_mainWidget, "selectConfiguration");
	m_mainLayout->addMultiCellWidget(m_selectConfiguration, 0, 2, 1, 1);
	m_selectConfiguration->show();
	if (m_selectGame)
	{
		delete m_selectGame;
		m_selectGame = 0;
	}

	connect(m_atlantikNetwork, SIGNAL(playerListClear()), m_selectConfiguration, SLOT(slotPlayerListClear()));
	connect(m_atlantikNetwork, SIGNAL(playerListAdd(QString, QString, QString)), m_selectConfiguration, SLOT(slotPlayerListAdd(QString, QString, QString)));
	connect(m_atlantikNetwork, SIGNAL(playerListEdit(QString, QString, QString)), m_selectConfiguration, SLOT(slotPlayerListEdit(QString, QString, QString)));
	connect(m_atlantikNetwork, SIGNAL(playerListDel(QString)), m_selectConfiguration, SLOT(slotPlayerListDel(QString)));

	connect(m_selectConfiguration, SIGNAL(startGame()), m_atlantikNetwork, SLOT(startGame()));
}

void Atlantik::initGame()
{
	// Create board widget and replace the game configuration widget.
	m_board = new AtlantikBoard(m_atlanticCore, 40, AtlantikBoard::Play, m_mainWidget, "board");
	m_board->setViewProperties(m_config.indicateUnowned, m_config.highliteUnowned, m_config.darkenMortgaged, m_config.quartzEffects, m_config.animateTokens);

	m_mainLayout->addMultiCellWidget(m_board, 0, 2, 1, 1);
	m_board->show();

	if (m_selectConfiguration)
	{
		delete m_selectConfiguration;
		m_selectConfiguration = 0;
	}

	connect(m_atlantikNetwork, SIGNAL(displayCard(QString, QString)), m_board, SLOT(displayCard(QString, QString)));
	connect(m_board, SIGNAL(tokenConfirmation(Estate *)), m_atlantikNetwork, SLOT(tokenConfirmation(Estate *)));
}

void Atlantik::gameStarted()
{
	PortfolioView *portfolioView;
	for (QPtrListIterator<PortfolioView> it(m_portfolioViews); *it; ++it)
		if ((portfolioView = dynamic_cast<PortfolioView*>(*it)))
			portfolioView->buildPortfolio();
}

void Atlantik::slotConfigure()
{
	if (m_configDialog == 0)
		m_configDialog = new ConfigDialog(this);
	m_configDialog->show();
	
	connect(m_configDialog, SIGNAL(okClicked()), this, SLOT(slotUpdateConfig()));
}

void Atlantik::slotUpdateConfig()
{
	KConfig *config=kapp->config();
	bool optBool, configChanged = false;
	QString optStr;

	optStr = m_configDialog->playerName();
	if (m_config.playerName != optStr)
	{
		m_config.playerName = optStr;
		m_atlantikNetwork->cmdName(optStr);
	}

	optBool = m_configDialog->indicateUnowned();
	if (m_config.indicateUnowned != optBool)
	{
		m_config.indicateUnowned = optBool;
		configChanged = true;
	}

	optBool = m_configDialog->highliteUnowned();
	if (m_config.highliteUnowned != optBool)
	{
		m_config.highliteUnowned = optBool;
		configChanged = true;
	}

	optBool = m_configDialog->darkenMortgaged();
	if (m_config.darkenMortgaged != optBool)
	{
		m_config.darkenMortgaged = optBool;
		configChanged = true;
	}

	optBool = m_configDialog->animateToken();
	if (m_config.animateTokens != optBool)
	{
		m_config.animateTokens = optBool;
		configChanged = true;
	}

	optBool = m_configDialog->quartzEffects();
	if (m_config.quartzEffects != optBool)
	{
		m_config.quartzEffects = optBool;
		configChanged = true;
	}

	config->setGroup("Personalization");
	config->writeEntry("PlayerName", m_config.playerName);

	config->setGroup("Board");
	config->writeEntry("IndicateUnowned", m_config.indicateUnowned);
	config->writeEntry("HighliteUnowned", m_config.highliteUnowned);
	config->writeEntry("DarkenMortgaged", m_config.darkenMortgaged);
	config->writeEntry("AnimateToken", m_config.animateTokens);
	config->writeEntry("QuartzEffects", m_config.quartzEffects);

	config->sync();

	if (configChanged && m_board)
		m_board->setViewProperties(m_config.indicateUnowned, m_config.highliteUnowned, m_config.darkenMortgaged, m_config.quartzEffects, m_config.animateTokens);
}

void Atlantik::slotSendMsg()
{
	m_atlantikNetwork->cmdChat(m_input->text());
	m_input->setText("");
}

void Atlantik::slotMsgInfo(QString msg)
{
	serverMsgsAppend(msg);
}

void Atlantik::slotMsgError(QString msg)
{
	serverMsgsAppend("ERR: " + msg);
}

void Atlantik::slotMsgChat(QString player, QString msg)
{
	serverMsgsAppend("<b>" + player + ":</b> " + msg);
}

void Atlantik::serverMsgsAppend(QString msg)
{
	// Use append, not setText(old+new) because that one doesn't wrap
	m_serverMsgs->append("<BR>" + msg);
	m_serverMsgs->ensureVisible(0, m_serverMsgs->contentsHeight());
}

void Atlantik::playerChanged()
{
	m_roll->setEnabled(m_playerSelf->canRoll());
	m_buyEstate->setEnabled(m_playerSelf->canBuy());
	m_auctionEstate->setEnabled(m_playerSelf->canBuy());

	// TODO: Should be more finetuned, but monopd doesn't send can_endturn can_usejailcard can_payjail can_jailroll yet
	m_endTurn->setEnabled(m_playerSelf->hasTurn() && !(m_playerSelf->canRoll() || m_playerSelf->canBuy() || m_playerSelf->inJail()));
	m_jailCard->setEnabled(m_playerSelf->hasTurn() && m_playerSelf->inJail());
	m_jailPay->setEnabled(m_playerSelf->hasTurn() && m_playerSelf->inJail());
	m_jailRoll->setEnabled(m_playerSelf->hasTurn() && m_playerSelf->inJail());
}
