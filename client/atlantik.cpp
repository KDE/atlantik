#include <qlineedit.h>
#include <qscrollbar.h>
#include <qcolor.h>

#include <kdebug.h>

#include <kstdgameaction.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
 
#include "atlantik.moc"
#include "board.h"

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <trade.h>
#include <auction.h>

#include <atlantik_network.h>

#include "selectserver_widget.h"
#include "selectgame_widget.h"
#include "selectconfiguration_widget.h"
#include "trade_widget.h"

#include "designer.h"

#include "config.h"

extern AtlantikConfig atlantikConfig;

Atlantik::Atlantik () : KMainWindow ()
{
	// Read application configuration
	readConfig();

	// Toolbar: Game
//	KStdGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
	KStdGameAction::quit(kapp, SLOT(closeAllWindows()), actionCollection(), "game_quit");

	// Toolbar: Settings
	KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());
	(void) new KAction(i18n("Gameboard &Designer"), CTRL+Key_D, this, SLOT(startDesigner()), actionCollection(), "designer");

	// Initialize pointers to 0L
	m_configDialog = 0;
	m_playerSelf = 0;

	// Game core
	m_atlanticCore = new AtlanticCore(this, "atlanticCore");

	// Network layer
	m_atlantikNetwork = new AtlantikNetwork(m_atlanticCore, this, "atlantikNetwork");

	connect(m_atlantikNetwork, SIGNAL(msgInfo(QString)), this, SLOT(slotMsgInfo(QString)));
	connect(m_atlantikNetwork, SIGNAL(msgError(QString)), this, SLOT(slotMsgError(QString)));
	connect(m_atlantikNetwork, SIGNAL(msgChat(QString, QString)), this, SLOT(slotMsgChat(QString, QString)));
	connect(m_atlantikNetwork, SIGNAL(msgStartGame(QString)), this, SLOT(slotMsgStartGame(QString)));

#ifndef USE_KDE
	connect(m_atlantikNetwork, SIGNAL(connected()), this, SLOT(slotNetworkConnected()));
	connect(m_atlantikNetwork, SIGNAL(error(int)), this, SLOT(slotNetworkError(int)));
#else
	connect(m_atlantikNetwork, SIGNAL(connectionSuccess()), this, SLOT(slotNetworkConnected()));
	connect(m_atlantikNetwork, SIGNAL(error(int)), this, SLOT(slotNetworkError(int)));
	connect(m_atlantikNetwork, SIGNAL(connectionFailed(int)), this, SLOT(slotNetworkConnected()));
#endif

	connect(m_atlantikNetwork, SIGNAL(joinedGame()), this, SLOT(slotJoinedGame()));
	connect(m_atlantikNetwork, SIGNAL(initGame()), this, SLOT(slotInitGame()));

	connect(m_atlantikNetwork, SIGNAL(newPlayer(Player *)), this, SLOT(newPlayer(Player *)));
	connect(m_atlantikNetwork, SIGNAL(newEstate(Estate *)), this, SLOT(newEstate(Estate *)));
	connect(m_atlantikNetwork, SIGNAL(newTrade(Trade *)), this, SLOT(newTrade(Trade *)));
	connect(m_atlantikNetwork, SIGNAL(newAuction(Auction *)), this, SLOT(newAuction(Auction *)));

	// Menu,toolbar: Move
	m_roll = KStdGameAction::roll(m_atlantikNetwork, SLOT(roll()), actionCollection()); // No Ctrl-R at the moment
	m_roll->setEnabled(false);
	m_buyEstate = new KAction(i18n("&Buy"), "atlantik_buy_estate", CTRL+Key_B, m_atlantikNetwork, SLOT(buyEstate()), actionCollection(), "buy_estate");
	m_buyEstate->setEnabled(false);
	m_auctionEstate = new KAction(i18n("&Auction"), "atlantik_auction_estate", CTRL+Key_A, m_atlantikNetwork, SLOT(auctionEstate()), actionCollection(), "auction_estate");
	m_auctionEstate->setEnabled(false);
	m_endTurn = KStdGameAction::endTurn(m_atlantikNetwork, SLOT(endTurn()), actionCollection());
	m_endTurn->setEnabled(false);
	m_jailCard = new KAction(i18n("Use card to leave jail"), "altantik_move_jail_card", 0, m_atlantikNetwork, SLOT(jailCard()), actionCollection(), "move_jailcard");
	m_jailCard->setEnabled(false);
	m_jailPay = new KAction(i18n("&Pay to leave jail"), "altantik_move_jail_pay", CTRL+Key_P, m_atlantikNetwork, SLOT(jailPay()), actionCollection(), "move_jailpay");
	m_jailPay->setEnabled(false);
	m_jailRoll = new KAction(i18n("&Roll to leave jail"), "altantik_move_jail_roll", CTRL+Key_R, m_atlantikNetwork, SLOT(jailRoll()), actionCollection(), "move_jailroll");
	m_jailRoll->setEnabled(false);

	// Mix code and XML into GUI
	createGUI();

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
	KConfig *config=kapp->config();

	// Personalization configuration
	config->setGroup("Personalization");
	atlantikConfig.playerName = config->readEntry("PlayerName", "Atlantik");

	// Board configuration
	config->setGroup("Board");
	atlantikConfig.indicateUnowned = config->readBoolEntry("IndicateUnowned", true);
	atlantikConfig.highliteUnowned = config->readBoolEntry("HighliteUnowned", false);
	atlantikConfig.darkenMortgaged = config->readBoolEntry("DarkenMortgaged", true);
	atlantikConfig.animateToken = config->readBoolEntry("AnimateToken", false);
	atlantikConfig.quartzEffects = config->readBoolEntry("QuartzEffects", true);
}

void Atlantik::newPlayer(Player *player)
{
	m_board->addToken(player);

	PortfolioView *portfolioView = new PortfolioView(player, m_portfolioWidget);
	m_portfolioViews.append(portfolioView);

	Estate *estate;
	QPtrList<Estate> estates = m_atlanticCore->estates();
	for (QPtrListIterator<Estate> it(estates); *it; ++it)
		if ((estate = dynamic_cast<Estate*>(*it)))
			portfolioView->addEstateView(estate);

	if (player->isSelf())
	{
		m_playerSelf = player;
		connect(player, SIGNAL(changed()), this, SLOT(playerChanged()));
	}
	connect(player, SIGNAL(changed()), portfolioView, SLOT(playerChanged()));
	connect(portfolioView, SIGNAL(newTrade(Player *)), m_atlantikNetwork, SLOT(newTrade(Player *)));

	m_portfolioLayout->addWidget(portfolioView);
	portfolioView->show();
}

void Atlantik::newEstate(Estate *estate)
{
	m_board->addEstateView(estate);

	PortfolioView *portfolioView;
	for (QPtrListIterator<PortfolioView> it(m_portfolioViews); *it; ++it)
		if ((portfolioView = dynamic_cast<PortfolioView*>(*it)))
			portfolioView->addEstateView(estate);
}

void Atlantik::newTrade(Trade *trade)
{
	TradeDisplay *tradeDisplay = new TradeDisplay(trade, m_atlanticCore, 0, "tradeDisplay");
	tradeDisplay->setFixedSize(200, 200);
	tradeDisplay->show();
						
//	QObject::connect(trade, SIGNAL(changed()), tradeDisplay, SLOT(tradeChanged()));

	// m_board->addTradeView(trade);
}

void Atlantik::newAuction(Auction *auction)
{
	m_board->addAuctionWidget(auction);
}

void Atlantik::slotNetworkConnected()
{
	// We're connected, so let's make ourselves known.
	m_atlantikNetwork->cmdName(atlantikConfig.playerName);

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

void Atlantik::slotNetworkError(int errno)
{
	QString errMsg(i18n("Error connecting: "));
	
#ifndef USE_KDE
	switch(errno)
	{
		case QSocket::ErrConnectionRefused:
			errMsg.append(i18n("connection refused by host."));
			break;

		case QSocket::ErrHostNotFound:
			errMsg.append(i18n("host not found."));
			break;

		case QSocket::ErrSocketRead:
			errMsg.append(i18n("could not read data."));
			break;

		default:
			errMsg.append(i18n("unknown error."));
	}
	serverMsgsAppend(errMsg);
#endif
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

void Atlantik::slotInitGame()
{
	// Create board widget and replace the game configuration widget.
	m_board = new AtlantikBoard(m_atlanticCore, 40, m_mainWidget, "board");
	m_mainLayout->addMultiCellWidget(m_board, 0, 2, 1, 1);
	m_board->show();
	if (m_selectConfiguration)
	{
		delete m_selectConfiguration;
		m_selectConfiguration = 0;
	}

	connect(m_atlantikNetwork, SIGNAL(displayCard(QString, QString)), m_board, SLOT(slotDisplayCard(QString, QString)));
	connect(m_board, SIGNAL(tokenConfirmation(Estate *)), m_atlantikNetwork, SLOT(tokenConfirmation(Estate *)));
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
	bool optBool, redrawEstates = false;
	QString optStr;

	optStr = m_configDialog->playerName();
	if (atlantikConfig.playerName != optStr)
	{
		atlantikConfig.playerName = optStr;
		m_atlantikNetwork->cmdName(optStr);
	}

	optBool = m_configDialog->indicateUnowned();
	if (atlantikConfig.indicateUnowned != optBool)
	{
		atlantikConfig.indicateUnowned = optBool;
		m_board->indicateUnownedChanged();
	}

	optBool = m_configDialog->highliteUnowned();
	if (atlantikConfig.highliteUnowned != optBool)
	{
		atlantikConfig.highliteUnowned = optBool;
		redrawEstates = true;
	}

	optBool = m_configDialog->darkenMortgaged();
	if (atlantikConfig.darkenMortgaged != optBool)
	{
		atlantikConfig.darkenMortgaged = optBool;
		redrawEstates = true;
	}

	optBool = m_configDialog->animateToken();
	if (atlantikConfig.animateToken != optBool)
	{
		atlantikConfig.animateToken = optBool;
	}

	optBool = m_configDialog->quartzEffects();
	if (atlantikConfig.quartzEffects != optBool)
	{
		atlantikConfig.quartzEffects = optBool;
		redrawEstates = true;
	}

	config->setGroup("Personalization");
	config->writeEntry("PlayerName", atlantikConfig.playerName);

	config->setGroup("Board");
	config->writeEntry("IndicateUnowned", atlantikConfig.indicateUnowned);
	config->writeEntry("HighliteUnowned", atlantikConfig.highliteUnowned);
	config->writeEntry("DarkenMortgaged", atlantikConfig.darkenMortgaged);
	config->writeEntry("AnimateToken", atlantikConfig.animateToken);
	config->writeEntry("QuartzEffects", atlantikConfig.quartzEffects);

	config->sync();

	if (redrawEstates)
	{
		Estate *estate;
		QPtrList<Estate> estates = m_atlanticCore->estates();
		for (QPtrListIterator<Estate> it(estates); *it; ++it)
			if ((estate = dynamic_cast<Estate*>(*it)))
				estate->update(true);
	}
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

void Atlantik::slotMsgStartGame(QString msg)
{
//	if (m_newgameWizard!=0)
//		m_newgameWizard->hide();
		
	serverMsgsAppend("START: " + msg);
}

void Atlantik::serverMsgsAppend(QString msg)
{
	// Use append, not setText(old+new) because that one doesn't wrap
	m_serverMsgs->append("<BR>" + msg);
	m_serverMsgs->ensureVisible(0, m_serverMsgs->contentsHeight());
}

void Atlantik::startDesigner()
{
	   AtlanticDesigner *designer = new AtlanticDesigner(this, "Designer");
	   designer->show();
}

void Atlantik::playerChanged()
{
	m_roll->setEnabled(m_playerSelf->canRoll());
	m_buyEstate->setEnabled(m_playerSelf->canBuy());
	m_auctionEstate->setEnabled(m_playerSelf->canBuy());
	m_endTurn->setEnabled(m_playerSelf->hasTurn() && !(m_playerSelf->canRoll() || m_playerSelf->canBuy()));

	// Could be more finetuned, but monopd doesn't send can_usejailcard can_payjail can_jailroll yet
	m_jailCard->setEnabled(m_playerSelf->inJail());
	m_jailPay->setEnabled(m_playerSelf->inJail());
	m_jailRoll->setEnabled(m_playerSelf->inJail());
}
