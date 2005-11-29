// Copyright (c) 2002-2004 Rob Kaper <cap@capsi.com>
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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include <errno.h>

#include <qcolor.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <qscrollview.h>
#include <qpopupmenu.h>

#include <kaboutapplication.h>
#include <kaction.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <knotifydialog.h>
#include <kstatusbar.h>
#include <kstdgameaction.h>
#include <kstdaction.h>
#include <ktoolbar.h>

#include <kdeversion.h>
#undef KDE_3_2_FEATURES
#if defined(KDE_MAKE_VERSION)
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
	#define KDE_3_2_FEATURES
#endif
#endif

#include <kdebug.h>

#include <atlantic_core.h>
#include <auction.h>
#include <estate.h>
#include <player.h>
#include <trade.h>
#include "atlantik.moc"

#include <atlantik_network.h>

#include <board.h>
#include <trade_widget.h>

#include "eventlogwidget.h"
#include "main.h"
#include "selectserver_widget.h"
#include "selectgame_widget.h"
#include "selectconfiguration_widget.h"

LogTextEdit::LogTextEdit( QWidget *parent, const char *name ) : QTextEdit( parent, name )
{
#ifdef KDE_3_2_FEATURES
	m_clear = KStdAction::clear( this, SLOT( clear() ), 0 );
#else
	m_clear = new KAction( i18n("Clear"), "clear", NULL, this, SLOT( clear() ), static_cast<KActionCollection *>(0), "clear" );
#endif
	m_selectAll = KStdAction::selectAll( this, SLOT( selectAll() ), 0 );
	m_copy = KStdAction::copy( this, SLOT( copy() ), 0 );
}

LogTextEdit::~LogTextEdit()
{
	delete m_clear;
	delete m_selectAll;
	delete m_copy;
}

QPopupMenu *LogTextEdit::createPopupMenu( const QPoint & )
{
	QPopupMenu *rmbMenu = new QPopupMenu( this );
	m_clear->plug( rmbMenu );
	rmbMenu->insertSeparator();
	m_copy->setEnabled( hasSelectedText() );
	m_copy->plug( rmbMenu );
	m_selectAll->plug( rmbMenu );

	return rmbMenu;
}

Atlantik::Atlantik ()
 :	KMainWindow (),
 	m_runningGame( false )
{
	// Read application configuration
	readConfig();

	// Toolbar: Game
//	KStdGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
        m_showEventLog = new KAction(i18n("Show Event &Log")/*, "atlantik_showeventlog"*/, CTRL+Key_L, this, SLOT(showEventLog()), actionCollection(), "showeventlog");
	KStdGameAction::quit(kapp, SLOT(closeAllWindows()), actionCollection(), "game_quit");

	// Toolbar: Settings
	KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());
	KStdAction::configureNotifications(this, SLOT(configureNotifications()), actionCollection());

	// Initialize pointers to 0L
	m_configDialog = 0;
	m_board = 0;
	m_eventLogWidget = 0;
	m_selectServer = 0;
	m_selectGame = 0;
	m_selectConfiguration = 0;
	m_atlantikNetwork = 0;

	// Game and network core
	m_atlanticCore = new AtlanticCore(this, "atlanticCore");
	connect(m_atlanticCore, SIGNAL(createGUI(Player *)), this, SLOT(newPlayer(Player *)));
	connect(m_atlanticCore, SIGNAL(removeGUI(Player *)), this, SLOT(removeGUI(Player *)));
	connect(m_atlanticCore, SIGNAL(createGUI(Trade *)), this, SLOT(newTrade(Trade *)));
	connect(m_atlanticCore, SIGNAL(removeGUI(Trade *)), this, SLOT(removeGUI(Trade *)));

	initEventLog();
	initNetworkObject();

	// Menu,toolbar: Move
	m_roll = KStdGameAction::roll(this, SIGNAL(rollDice()), actionCollection());
	m_roll->setEnabled(false);
	m_buyEstate = new KAction(i18n("&Buy"), "atlantik_buy_estate", CTRL+Key_B, this, SIGNAL(buyEstate()), actionCollection(), "buy_estate");
	m_buyEstate->setEnabled(false);
	m_auctionEstate = new KAction(i18n("&Auction"), "auction", CTRL+Key_A, this, SIGNAL(auctionEstate()), actionCollection(), "auction");
	m_auctionEstate->setEnabled(false);
	m_endTurn = KStdGameAction::endTurn(this, SIGNAL(endTurn()), actionCollection());
	m_endTurn->setEnabled(false);
        m_jailCard = new KAction(i18n("Use Card to Leave Jail")/*, "atlantik_move_jail_card"*/, 0, this, SIGNAL(jailCard()), actionCollection(), "move_jailcard");
	m_jailCard->setEnabled(false);
	m_jailPay = new KAction(i18n("&Pay to Leave Jail"), "jail_pay", CTRL+Key_P, this, SIGNAL(jailPay()), actionCollection(), "move_jailpay");
	m_jailPay->setEnabled(false);
        m_jailRoll = new KAction(i18n("Roll to Leave &Jail")/*, "atlantik_move_jail_roll"*/, CTRL+Key_J, this, SIGNAL(jailRoll()), actionCollection(), "move_jailroll");
	m_jailRoll->setEnabled(false);

	// Mix code and XML into GUI
	KMainWindow::createGUI();
	applyMainWindowSettings( KGlobal::config(), "AtlantikMainWindow" );
	KMainWindow::statusBar()->insertItem("Atlantik " ATLANTIK_VERSION_STRING, 0);
	KMainWindow::statusBar()->insertItem(QString::null, 1);
	connect(statusBar(), SIGNAL(released(int)), this, SLOT(statusBarClick(int)));

	// Main widget, containing all others
 	m_mainWidget = new QWidget(this, "main");
	m_mainWidget->show();
	m_mainLayout = new QGridLayout(m_mainWidget, 3, 2);
	setCentralWidget(m_mainWidget);

	// Vertical view area for portfolios.
	m_portfolioScroll = new QScrollView(m_mainWidget, "pfScroll");
	m_mainLayout->addWidget( m_portfolioScroll, 0, 0 );
	m_portfolioScroll->setHScrollBarMode( QScrollView::AlwaysOff );
	m_portfolioScroll->setResizePolicy( QScrollView::AutoOneFit );
	m_portfolioScroll->setFixedHeight( 200 );
	m_portfolioScroll->hide();

	m_portfolioWidget = new QWidget( m_portfolioScroll->viewport(), "pfWidget" );
	m_portfolioScroll->addChild( m_portfolioWidget );
	m_portfolioWidget->show();

	m_portfolioLayout = new QVBoxLayout(m_portfolioWidget);
	m_portfolioViews.setAutoDelete(true);

	// Nice label
//	m_portfolioLabel = new QLabel(i18n("Players"), m_portfolioWidget, "pfLabel");
//	m_portfolioLayout->addWidget(m_portfolioLabel);
//	m_portfolioLabel->show();

	// Text view for chat and status messages from server.
	m_serverMsgs = new LogTextEdit(m_mainWidget, "serverMsgs");
	m_serverMsgs->setTextFormat(QTextEdit::PlainText);
	m_serverMsgs->setReadOnly(true);
	m_serverMsgs->setHScrollBarMode(QScrollView::AlwaysOff);
	m_serverMsgs->setMinimumWidth(200);
	m_mainLayout->addWidget(m_serverMsgs, 1, 0);

	// LineEdit to enter commands and chat messages.
	m_input = new QLineEdit(m_mainWidget, "input");
	m_mainLayout->addWidget(m_input, 2, 0);

	m_serverMsgs->setFocusProxy(m_input);

	connect(m_input, SIGNAL(returnPressed()), this, SLOT(slotSendMsg()));

	// Set stretching where we want it.
	m_mainLayout->setRowStretch(1, 1); // make m_board+m_serverMsgs stretch vertically, not the rest
	m_mainLayout->setColStretch(1, 1); // make m_board stretch horizontally, not the rest

	// Check command-line args to see if we need to connect or show Monopigator window
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	QCString host = args->getOption("host");	
	QCString port = args->getOption("port");	
	if (!host.isNull() && !port.isNull())
		m_atlantikNetwork->serverConnect(host, port.toInt());
	else
		showSelectServer();
}

void Atlantik::readConfig()
{
	// Read configuration settings
	KConfig *config = kapp->config();

	// General configuration
	config->setGroup("General");
	m_config.chatTimestamps = config->readBoolEntry("ChatTimeStamps", false);

	// Personalization configuration
	config->setGroup("Personalization");
	m_config.playerName = config->readEntry("PlayerName", "Atlantik");
	m_config.playerImage = config->readEntry("PlayerImage", "cube.png");

	// Board configuration
	config->setGroup("Board");
	m_config.indicateUnowned = config->readBoolEntry("IndicateUnowned", true);
	m_config.highliteUnowned = config->readBoolEntry("HighliteUnowned", false);
	m_config.darkenMortgaged = config->readBoolEntry("DarkenMortgaged", true);
	m_config.animateTokens = config->readBoolEntry("AnimateToken", false);
	m_config.quartzEffects = config->readBoolEntry("QuartzEffects", true);

	// Meta server configuation
	config->setGroup("Monopigator");
	m_config.connectOnStart = config->readBoolEntry("ConnectOnStart", false);
	m_config.hideDevelopmentServers = config->readBoolEntry("HideDevelopmentServers", true);

	// Portfolio colors
	config->setGroup("WM");
	QColor activeDefault(204, 204, 204), inactiveDefault(153, 153, 153);
	m_config.activeColor = config->readColorEntry("activeBackground", &activeDefault);
	m_config.inactiveColor = config->readColorEntry("inactiveBlend", &inactiveDefault);
}

void Atlantik::newPlayer(Player *player)
{
	initBoard();
	m_board->addToken(player);
	addPortfolioView(player);

	// Player::changed() is not connected until later this method, so
	// we'd better force an update.
	playerChanged(player);

	connect(player, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
	connect(player, SIGNAL(gainedTurn()), this, SLOT(gainedTurn()));
	connect(player, SIGNAL(changed(Player *)), m_board, SLOT(playerChanged(Player *)));

	KNotifyClient::event(winId(), "newplayer");
}

void Atlantik::newEstate(Estate *estate)
{
	initBoard();
	m_board->addEstateView(estate, m_config.indicateUnowned, m_config.highliteUnowned, m_config.darkenMortgaged, m_config.quartzEffects);
}

void Atlantik::newTrade(Trade *trade)
{
	TradeDisplay *tradeDisplay = new TradeDisplay(trade, m_atlanticCore, 0, "tradeDisplay");
	m_tradeGUIMap[trade] = tradeDisplay;
	tradeDisplay->show();
}

void Atlantik::newAuction(Auction *auction)
{
	initBoard();
	m_board->addAuctionWidget(auction);
}

void Atlantik::removeGUI(Player *player)
{
	// Find and remove portfolioview
	PortfolioView *portfolioView = findPortfolioView(player);
	if (portfolioView)
		m_portfolioViews.remove(portfolioView);

	if (m_board)
		m_board->removeToken(player);
}

void Atlantik::removeGUI(Trade *trade)
{
	if (TradeDisplay *tradeDisplay = m_tradeGUIMap[trade])
		delete tradeDisplay;
}

void Atlantik::showSelectServer()
{
	if (m_selectServer)
		return;

	m_selectServer = new SelectServer(m_config.connectOnStart, m_config.hideDevelopmentServers, m_mainWidget, "selectServer");
	m_mainLayout->addMultiCellWidget(m_selectServer, 0, 2, 1, 1);
	m_selectServer->show();

	if (m_selectGame)
	{
		delete m_selectGame;
		m_selectGame = 0;
	}

	m_atlanticCore->reset(true);
	initNetworkObject();

	connect(m_selectServer, SIGNAL(serverConnect(const QString, int)), m_atlantikNetwork, SLOT(serverConnect(const QString, int)));
	connect(m_selectServer, SIGNAL(msgStatus(const QString &)), this, SLOT(slotMsgStatus(const QString &)));

	m_selectServer->slotRefresh( m_config.connectOnStart );
}

void Atlantik::showSelectGame()
{
	if (m_selectGame)
		return;

	m_selectGame = new SelectGame(m_atlanticCore, m_mainWidget, "selectGame");
	m_atlanticCore->emitGames();

	m_mainLayout->addMultiCellWidget(m_selectGame, 0, 2, 1, 1);
	m_selectGame->show();

	// Reset core and GUI
	if (m_board)
	{
		m_board->hide();
		m_board->reset();
//		delete m_board;
//		m_board = 0;

		// m_portfolioViews.clear();
		m_atlanticCore->reset();
	}

	if (m_selectServer)
	{
		delete m_selectServer;
		m_selectServer = 0;
	}
	if (m_selectConfiguration)
	{
		delete m_selectConfiguration;
		m_selectConfiguration = 0;
	}

	connect(m_selectGame, SIGNAL(joinGame(int)), m_atlantikNetwork, SLOT(joinGame(int)));
	connect(m_selectGame, SIGNAL(newGame(const QString &)), m_atlantikNetwork, SLOT(newGame(const QString &)));
	connect(m_selectGame, SIGNAL(leaveServer()), this, SLOT(showSelectServer()));
	connect(m_selectGame, SIGNAL(msgStatus(const QString &)), this, SLOT(slotMsgStatus(const QString &)));
}

void Atlantik::showSelectConfiguration()
{
	if (m_selectConfiguration)
		return;

	if (m_selectGame)
	{
		delete m_selectGame;
		m_selectGame = 0;
	}

	m_selectConfiguration = new SelectConfiguration(m_atlanticCore, m_mainWidget, "selectConfiguration");
	m_mainLayout->addMultiCellWidget(m_selectConfiguration, 0, 2, 1, 1);
	m_selectConfiguration->show();

	connect(m_atlanticCore, SIGNAL(createGUI(ConfigOption *)), m_selectConfiguration, SLOT(addConfigOption(ConfigOption *)));
	connect(m_atlantikNetwork, SIGNAL(gameOption(QString, QString, QString, QString, QString)), m_selectConfiguration, SLOT(gameOption(QString, QString, QString, QString, QString)));
	connect(m_atlantikNetwork, SIGNAL(gameInit()), m_selectConfiguration, SLOT(initGame()));
	connect(m_selectConfiguration, SIGNAL(startGame()), m_atlantikNetwork, SLOT(startGame()));
	connect(m_selectConfiguration, SIGNAL(leaveGame()), m_atlantikNetwork, SLOT(leaveGame()));
	connect(m_selectConfiguration, SIGNAL(changeOption(int, const QString &)), m_atlantikNetwork, SLOT(changeOption(int, const QString &)));
	connect(m_selectConfiguration, SIGNAL(buttonCommand(QString)), m_atlantikNetwork, SLOT(writeData(QString)));
	connect(m_selectConfiguration, SIGNAL(iconSelected(const QString &)), m_atlantikNetwork, SLOT(setImage(const QString &)));
	connect(m_selectConfiguration, SIGNAL(statusMessage(const QString &)), this, SLOT(slotMsgStatus(const QString &)));
}

void Atlantik::initBoard()
{
	if (m_board)
		return;

	m_board = new AtlantikBoard(m_atlanticCore, 40, AtlantikBoard::Play, m_mainWidget, "board");
	m_board->setViewProperties(m_config.indicateUnowned, m_config.highliteUnowned, m_config.darkenMortgaged, m_config.quartzEffects, m_config.animateTokens);

	connect(m_atlantikNetwork, SIGNAL(displayDetails(QString, bool, bool, Estate *)), m_board, SLOT(insertDetails(QString, bool, bool, Estate *)));
	connect(m_atlantikNetwork, SIGNAL(addCommandButton(QString, QString, bool)), m_board, SLOT(displayButton(QString, QString, bool)));
	connect(m_atlantikNetwork, SIGNAL(addCloseButton()), m_board, SLOT(addCloseButton()));
	connect(m_board, SIGNAL(tokenConfirmation(Estate *)), m_atlantikNetwork, SLOT(tokenConfirmation(Estate *)));
	connect(m_board, SIGNAL(buttonCommand(QString)), m_atlantikNetwork, SLOT(writeData(QString)));
}

void Atlantik::showBoard()
{
	if (m_selectGame)
	{
		delete m_selectGame;
		m_selectGame = 0;
	}

	if (m_selectConfiguration)
	{
		delete m_selectConfiguration;
		m_selectConfiguration = 0;
	}

	if (!m_board)
		initBoard();

	m_runningGame = true;

	m_mainLayout->addMultiCellWidget(m_board, 0, 2, 1, 1);
	m_board->displayDefault();
	m_board->show();

	PortfolioView *portfolioView = 0;
	for (QPtrListIterator<PortfolioView> it(m_portfolioViews); *it; ++it)
		if ((portfolioView = dynamic_cast<PortfolioView*>(*it)))
			portfolioView->buildPortfolio();
}

void Atlantik::freezeBoard()
{
	if (!m_board)
		showBoard();

	m_runningGame = false;
	// TODO: m_board->freeze();
}

void Atlantik::slotNetworkConnected()
{
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

	// Re-init network object
	initNetworkObject();
}

void Atlantik::networkClosed(int status)
{
	switch( status )
	{
	case KBufferedIO::involuntary:
		slotMsgStatus( i18n("Connection with server %1:%2 lost.").arg(m_atlantikNetwork->host()).arg(m_atlantikNetwork->port()), QString("connect_no") );
		showSelectServer();
		break;
	default:
		if ( !m_atlantikNetwork->host().isEmpty() )
			slotMsgStatus( i18n("Disconnected from %1:%2.").arg(m_atlantikNetwork->host()).arg(m_atlantikNetwork->port()), QString("connect_no") );
		break;
	}
}

void Atlantik::slotConfigure()
{
	if (m_configDialog == 0)
		m_configDialog = new ConfigDialog(this);
	m_configDialog->show();
	
	connect(m_configDialog, SIGNAL(okClicked()), this, SLOT(slotUpdateConfig()));
}

void Atlantik::showEventLog()
{
	if (!m_eventLogWidget)
		m_eventLogWidget = new EventLogWidget(m_eventLog, 0);
	m_eventLogWidget->show();
}

void Atlantik::configureNotifications()
{
	KNotifyDialog::configure(this);
}

void Atlantik::slotUpdateConfig()
{
	KConfig *config=kapp->config();
	bool optBool, configChanged = false;
	QString optStr;

	optBool = m_configDialog->chatTimestamps();
	if (m_config.chatTimestamps != optBool)
	{
		m_config.chatTimestamps = optBool;
		configChanged = true;
	}

	optStr = m_configDialog->playerName();
	if (m_config.playerName != optStr)
	{
		m_config.playerName = optStr;
		m_atlantikNetwork->setName(optStr);
	}

	optStr = m_configDialog->playerImage();
	if (m_config.playerImage != optStr)
	{
		m_config.playerImage = optStr;
		m_atlantikNetwork->setImage(optStr);
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

	optBool = m_configDialog->connectOnStart();
	if (m_config.connectOnStart != optBool)
	{
		m_config.connectOnStart = optBool;
		configChanged = true;
	}

	optBool = m_configDialog->hideDevelopmentServers();
	if (m_config.hideDevelopmentServers != optBool)
	{
		m_config.hideDevelopmentServers = optBool;
		if (m_selectServer)
			m_selectServer->setHideDevelopmentServers(optBool);

		configChanged = true;
	}

	config->setGroup("General");
	config->writeEntry("ChatTimeStamps", m_config.chatTimestamps);

	config->setGroup("Personalization");
	config->writeEntry("PlayerName", m_config.playerName);
	config->writeEntry("PlayerImage", m_config.playerImage);

	config->setGroup("Board");
	config->writeEntry("IndicateUnowned", m_config.indicateUnowned);
	config->writeEntry("HighliteUnowned", m_config.highliteUnowned);
	config->writeEntry("DarkenMortgaged", m_config.darkenMortgaged);
	config->writeEntry("AnimateToken", m_config.animateTokens);
	config->writeEntry("QuartzEffects", m_config.quartzEffects);

	config->setGroup("Monopigator");
	config->writeEntry("ConnectOnStart", m_config.connectOnStart);
	config->writeEntry("HideDevelopmentServers", m_config.hideDevelopmentServers);

	config->sync();

	if (configChanged && m_board)
		m_board->setViewProperties(m_config.indicateUnowned, m_config.highliteUnowned, m_config.darkenMortgaged, m_config.quartzEffects, m_config.animateTokens);
}

void Atlantik::slotSendMsg()
{
	m_atlantikNetwork->cmdChat(m_input->text());
	m_input->setText(QString::null);
}

void Atlantik::slotMsgInfo(QString msg)
{
	serverMsgsAppend(msg);
}

void Atlantik::slotMsgError(QString msg)
{
	serverMsgsAppend("Error: " + msg);
}

void Atlantik::slotMsgStatus(const QString &message, const QString &icon)
{
	KMainWindow::statusBar()->changeItem(message, 1);
	m_eventLog->addEvent(message, icon);
}

void Atlantik::slotMsgChat(QString player, QString msg)
{
	if (m_config.chatTimestamps)
	{
		QTime time = QTime::currentTime();
		serverMsgsAppend(QString("[%1] %2: %3").arg(time.toString("hh:mm")).arg(player).arg(msg));
	}
	else
		serverMsgsAppend(player + ": " + msg);
	KNotifyClient::event(winId(), "chat");
}

void Atlantik::serverMsgsAppend(QString msg)
{
	// Use append, not setText(old+new) because that one doesn't wrap
	m_serverMsgs->append(msg);
	m_serverMsgs->ensureVisible(0, m_serverMsgs->contentsHeight());
}

void Atlantik::playerChanged(Player *player)
{
	PortfolioView *portfolioView = findPortfolioView(player);
	if (!portfolioView)
		portfolioView = addPortfolioView(player);

	Player *playerSelf = m_atlanticCore->playerSelf();
	if (player == playerSelf)
	{
		// We changed ourselves..
		PortfolioView *portfolioView = 0;
		for (QPtrListIterator<PortfolioView> it(m_portfolioViews); *it; ++it)
			if ((portfolioView = dynamic_cast<PortfolioView*>(*it)))
			{
				// Clear all portfolios if we're not in game
				if ( !player->game() )
					portfolioView->clearPortfolio();

				// Show players in our game, hide the rest
				Player *pTmp = portfolioView->player();
				if (pTmp->game() == playerSelf->game())
					portfolioView->show();
				else
					portfolioView->hide();
			}
		if (!player->game())
			showSelectGame();
		else
		{
			if ( !m_board || m_board->isHidden() )
				showSelectConfiguration();
		}

		m_roll->setEnabled(player->canRoll());
		m_buyEstate->setEnabled(player->canBuy());
		m_auctionEstate->setEnabled(player->canAuction());

		// TODO: Should be more finetuned, but monopd doesn't send can_endturn can_payjail can_jailroll yet
		m_endTurn->setEnabled(player->hasTurn() && !(player->canRoll() || player->canBuy() || player->inJail()));
		m_jailCard->setEnabled(player->canUseCard());
		m_jailPay->setEnabled(player->hasTurn() && player->inJail());
		m_jailRoll->setEnabled(player->hasTurn() && player->inJail());
	}
	else
	{
		// Another player changed, check if we need to show or hide
		// his/her portfolioView.
		if (playerSelf)
		{
			if (player->game() == playerSelf->game())
				portfolioView->show();
			else
				portfolioView->hide();
		}
		else if ( !player->game() )
			portfolioView->hide();
	}
}

void Atlantik::gainedTurn()
{
	KNotifyClient::event(winId(), "gainedturn", i18n("It is your turn now.") );
}

void Atlantik::initEventLog()
{
	m_eventLog = new EventLog();
}

void Atlantik::initNetworkObject()
{
	if (m_atlantikNetwork)
	{
		m_atlantikNetwork->reset();
		return;
	}

	m_atlantikNetwork = new AtlantikNetwork(m_atlanticCore);
	connect(m_atlantikNetwork, SIGNAL(msgInfo(QString)), this, SLOT(slotMsgInfo(QString)));
	connect(m_atlantikNetwork, SIGNAL(msgError(QString)), this, SLOT(slotMsgError(QString)));
	connect(m_atlantikNetwork, SIGNAL(msgStatus(const QString &, const QString &)), this, SLOT(slotMsgStatus(const QString &, const QString &)));
	connect(m_atlantikNetwork, SIGNAL(msgChat(QString, QString)), this, SLOT(slotMsgChat(QString, QString)));

	connect(m_atlantikNetwork, SIGNAL(connectionSuccess()), this, SLOT(slotNetworkConnected()));
	connect(m_atlantikNetwork, SIGNAL(connectionFailed(int)), this, SLOT(slotNetworkError(int)));
	connect(m_atlantikNetwork, SIGNAL(closed(int)), this, SLOT(networkClosed(int)));

	connect(m_atlantikNetwork, SIGNAL(receivedHandshake()), this, SLOT(sendHandshake()));

	connect(m_atlantikNetwork, SIGNAL(gameConfig()), this, SLOT(showSelectConfiguration()));
	connect(m_atlantikNetwork, SIGNAL(gameInit()), this, SLOT(initBoard()));
	connect(m_atlantikNetwork, SIGNAL(gameRun()), this, SLOT(showBoard()));
	connect(m_atlantikNetwork, SIGNAL(gameEnd()), this, SLOT(freezeBoard()));

	connect(m_atlantikNetwork, SIGNAL(newEstate(Estate *)), this, SLOT(newEstate(Estate *)));
	connect(m_atlantikNetwork, SIGNAL(newAuction(Auction *)), this, SLOT(newAuction(Auction *)));

	connect(m_atlantikNetwork, SIGNAL(clientCookie(QString)), this, SLOT(clientCookie(QString)));
	connect(m_atlantikNetwork, SIGNAL(networkEvent(const QString &, const QString &)), m_eventLog, SLOT(addEvent(const QString &, const QString &)));

	connect(this, SIGNAL(rollDice()), m_atlantikNetwork, SLOT(rollDice()));
	connect(this, SIGNAL(buyEstate()), m_atlantikNetwork, SLOT(buyEstate()));
	connect(this, SIGNAL(auctionEstate()), m_atlantikNetwork, SLOT(auctionEstate()));
	connect(this, SIGNAL(endTurn()), m_atlantikNetwork, SLOT(endTurn()));
	connect(this, SIGNAL(jailCard()), m_atlantikNetwork, SLOT(jailCard()));
	connect(this, SIGNAL(jailPay()), m_atlantikNetwork, SLOT(jailPay()));
	connect(this, SIGNAL(jailRoll()), m_atlantikNetwork, SLOT(jailRoll()));
}

void Atlantik::clientCookie(QString cookie)
{
	KConfig *config = kapp->config();

	if (cookie.isNull())
	{
		if (config->hasGroup("Reconnection"))
			config->deleteGroup("Reconnection", true);
	}
	else if (m_atlantikNetwork)
	{
		config->setGroup("Reconnection");
		config->writeEntry("Host", m_atlantikNetwork->host());
		config->writeEntry("Port", m_atlantikNetwork->port());
		config->writeEntry("Cookie", cookie);
	}
	else
		return;

	config->sync();
}

void Atlantik::sendHandshake()
{
	m_atlantikNetwork->setName(m_config.playerName);
	m_atlantikNetwork->setImage(m_config.playerImage);

	// Check command-line args to see if we need to auto-join
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	QCString game = args->getOption("game");	
	if (!game.isNull())
		m_atlantikNetwork->joinGame(game.toInt());
}

void Atlantik::statusBarClick(int item)
{
	if ( item == 0 )
	{
		KAboutApplication dialog(kapp->aboutData(), this);
		dialog.exec();
	}
	else if ( item == 1)
		showEventLog();
}

PortfolioView *Atlantik::addPortfolioView(Player *player)
{
	PortfolioView *portfolioView = new PortfolioView(m_atlanticCore, player, m_config.activeColor, m_config.inactiveColor, m_portfolioWidget);
	m_portfolioViews.append(portfolioView);
	if ( m_portfolioViews.count() > 0 && m_portfolioScroll->isHidden() )
		m_portfolioScroll->show();

	connect(player, SIGNAL(changed(Player *)), portfolioView, SLOT(playerChanged()));
	connect(portfolioView, SIGNAL(newTrade(Player *)), m_atlantikNetwork, SLOT(newTrade(Player *)));
	connect(portfolioView, SIGNAL(kickPlayer(Player *)), m_atlantikNetwork, SLOT(kickPlayer(Player *)));
	connect(portfolioView, SIGNAL(estateClicked(Estate *)), m_board, SLOT(prependEstateDetails(Estate *)));

	m_portfolioLayout->addWidget(portfolioView);
	portfolioView->show();

	return portfolioView;
}

PortfolioView *Atlantik::findPortfolioView(Player *player)
{
	PortfolioView *portfolioView = 0;
	for (QPtrListIterator<PortfolioView> it(m_portfolioViews); (portfolioView = *it) ; ++it)
		if (player == portfolioView->player())
			return portfolioView;

	return 0;
}

void Atlantik::closeEvent(QCloseEvent *e)
{
	Game *gameSelf = m_atlanticCore->gameSelf();
	Player *playerSelf = m_atlanticCore->playerSelf();

	int result = KMessageBox::Continue;
	if ( gameSelf && !playerSelf->isBankrupt() && m_runningGame )
		result = KMessageBox::warningContinueCancel( this, i18n("You are currently part of an active game. Are you sure you want to close Atlantik? If you do, you forfeit the game."), i18n("Close & Forfeit?"), i18n("Close && Forfeit") );

	if ( result == KMessageBox::Continue )
	{
		if ( m_atlantikNetwork )
			m_atlantikNetwork->leaveGame();

		saveMainWindowSettings(kapp->config(), "AtlantikMainWindow");
		KMainWindow::closeEvent(e);
	}
}
