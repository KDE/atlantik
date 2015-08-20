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

#include <QColor>
#include <QDateTime>
#include <QLineEdit>
#include <q3scrollview.h>
#include <qmenu.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QGridLayout>
#include <QCloseEvent>

#include <kaboutapplicationdialog.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knotifyconfigwidget.h>
#include <knotification.h>
#include <kstatusbar.h>
#include <kstandardgameaction.h>
#include <kstandardaction.h>
#include <ktoolbar.h>

#include <kdeversion.h>

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
#include <kglobal.h>

#include "eventlogwidget.h"
#include "main.h"
#include "selectserver_widget.h"
#include "selectgame_widget.h"
#include "selectconfiguration_widget.h"

LogTextEdit::LogTextEdit(QWidget *parent) : QTextEdit(parent)
{
	m_clear = KStandardAction::clear( this, SLOT( clear() ), 0 );
	m_selectAll = KStandardAction::selectAll( this, SLOT( selectAll() ), 0 );
	m_copy = KStandardAction::copy( this, SLOT( copy() ), 0 );
	connect(this, SIGNAL(copyAvailable(bool)), m_copy, SLOT(setEnabled(bool)));
	m_copy->setEnabled(false);
}

LogTextEdit::~LogTextEdit()
{
	delete m_clear;
	delete m_selectAll;
	delete m_copy;
}

void LogTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	menu.addAction(m_clear);
	menu.addSeparator();
	menu.addAction(m_copy);
	menu.addAction(m_selectAll);
	menu.exec(event->globalPos());
}

Atlantik::Atlantik ()
 :	KXmlGuiWindow (),
 	m_runningGame( false )
{
	// Read application configuration
	readConfig();

	// Toolbar: Game
//	KStandardGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
        m_showEventLog = actionCollection()->addAction("showeventlog");
        m_showEventLog->setText(i18n("Show Event &Log"));
		//m_showEventLog->setShortcut(KStandardShortcut::shortcut(KStandardShortcut::New));
		connect(m_showEventLog, SIGNAL(triggered(bool)), this, SLOT(showEventLog()));
        QAction *act = KStandardGameAction::quit(kapp, SLOT(closeAllWindows()), this);
        actionCollection()->addAction("game_quit", act);

	// Toolbar: Settings
	act = KStandardAction::preferences(this, SLOT(slotConfigure()), this);
        actionCollection()->addAction(act->objectName(), act);
	act = KStandardAction::configureNotifications(this, SLOT(configureNotifications()), this);
        actionCollection()->addAction(act->objectName(), act);

	// Initialize pointers to 0L
	m_configDialog = 0;
	m_board = 0;
	m_eventLogWidget = 0;
	m_selectServer = 0;
	m_selectGame = 0;
	m_selectConfiguration = 0;
	m_atlantikNetwork = 0;

	// Game and network core
	m_atlanticCore = new AtlanticCore(this);
        m_atlanticCore->setObjectName("atlanticCore");
	connect(m_atlanticCore, SIGNAL(createGUI(Player *)), this, SLOT(newPlayer(Player *)));
	connect(m_atlanticCore, SIGNAL(removeGUI(Player *)), this, SLOT(removeGUI(Player *)));
	connect(m_atlanticCore, SIGNAL(createGUI(Trade *)), this, SLOT(newTrade(Trade *)));
	connect(m_atlanticCore, SIGNAL(removeGUI(Trade *)), this, SLOT(removeGUI(Trade *)));

	initEventLog();
	initNetworkObject();

	// Menu,toolbar: Move
	m_roll = KStandardGameAction::roll(this, SIGNAL(rollDice()), this);
        actionCollection()->addAction(m_roll->objectName(), m_roll);
	m_roll->setEnabled(false);

	m_buyEstate = actionCollection()->addAction("atlantik_buy_estate");
        m_buyEstate->setText(i18n("&Buy"));
	m_buyEstate->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_B) );
	connect (m_buyEstate,SIGNAL(toggled(bool)), this, SIGNAL(buyEstate()));
	m_buyEstate->setEnabled(false);

	m_auctionEstate = actionCollection()->addAction("auction");
        m_auctionEstate->setText(i18n("&Auction"));
	m_auctionEstate->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_A));
	connect(m_auctionEstate,SIGNAL(toggled(bool)),this, SIGNAL(auctionEstate()));
	m_auctionEstate->setEnabled(false);


	m_endTurn = KStandardGameAction::endTurn(this, SIGNAL(endTurn()), this);
        actionCollection()->addAction(m_endTurn->objectName(), m_endTurn);
	m_endTurn->setEnabled(false);

	m_jailCard = actionCollection()->addAction("move_jailcard");
        m_jailCard->setText(i18n("Use Card to Leave Jail"));
	connect(m_jailCard, SIGNAL(toggled(bool)),this, SIGNAL(jailCard()));
	m_jailCard->setEnabled(false);

	m_jailPay = actionCollection()->addAction("jail_pay");
        m_jailPay->setText(i18n("&Pay to Leave Jail"));
	m_jailPay->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	connect(m_jailPay, SIGNAL(toggled(bool)),this, SIGNAL(jailPay()));
	m_jailPay->setEnabled(false);

	m_jailRoll = actionCollection()->addAction("move_jailroll");
        m_jailRoll->setText(i18n("Roll to Leave &Jail"));
	m_jailRoll->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));
	connect(m_jailRoll, SIGNAL(toggled(bool)), this, SIGNAL(jailRoll()));
	m_jailRoll->setEnabled(false);

	// Mix code and XML into GUI
	KXmlGuiWindow::createGUI();
	applyMainWindowSettings( KGlobal::config()->group( "AtlantikMainWindow" ) );
	KXmlGuiWindow::statusBar()->insertItem("Atlantik " ATLANTIK_VERSION_STRING, 0);
	KXmlGuiWindow::statusBar()->insertItem(QString::null, 1);
	connect(statusBar(), SIGNAL(released(int)), this, SLOT(statusBarClick(int)));

	// Main widget, containing all others
 	m_mainWidget = new QWidget(this);
        m_mainWidget->setObjectName( "main" );
	m_mainWidget->show();
	m_mainLayout = new QGridLayout(m_mainWidget);
	setCentralWidget(m_mainWidget);

	// Vertical view area for portfolios.
	m_portfolioScroll = new Q3ScrollView(m_mainWidget, "pfScroll");
	m_mainLayout->addWidget( m_portfolioScroll, 0, 0 );
	m_portfolioScroll->setHScrollBarMode( Q3ScrollView::AlwaysOff );
	m_portfolioScroll->setResizePolicy( Q3ScrollView::AutoOneFit );
	m_portfolioScroll->setFixedHeight( 200 );
	m_portfolioScroll->hide();

	m_portfolioWidget = new QWidget( m_portfolioScroll->viewport() );
        m_portfolioWidget->setObjectName( "pfWidget" );
	m_portfolioScroll->addChild( m_portfolioWidget );
	m_portfolioWidget->show();

	m_portfolioLayout = new QVBoxLayout(m_portfolioWidget);
	m_portfolioViews.setAutoDelete(true);

	// Nice label
//	m_portfolioLabel = new QLabel(i18n("Players"), m_portfolioWidget, "pfLabel");
//	m_portfolioLayout->addWidget(m_portfolioLabel);
//	m_portfolioLabel->show();

	// Text view for chat and status messages from server.
	m_serverMsgs = new LogTextEdit(m_mainWidget);
	m_serverMsgs->setObjectName(QLatin1String("serverMsgs"));
	m_serverMsgs->setAcceptRichText(false);
	m_serverMsgs->setReadOnly(true);
	m_serverMsgs->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_serverMsgs->setMinimumWidth(200);
	m_mainLayout->addWidget(m_serverMsgs, 1, 0);

	// LineEdit to enter commands and chat messages.
	m_input = new QLineEdit(m_mainWidget);
	m_input->setObjectName("input");
	m_mainLayout->addWidget(m_input, 2, 0);

	m_serverMsgs->setFocusProxy(m_input);

	connect(m_input, SIGNAL(returnPressed()), this, SLOT(slotSendMsg()));

	// Set stretching where we want it.
	m_mainLayout->setRowStretch(1, 1); // make m_board+m_serverMsgs stretch vertically, not the rest
	m_mainLayout->setColumnStretch(1, 1); // make m_board stretch horizontally, not the rest

	// Check command-line args to see if we need to connect or show Monopigator window
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	QString host = args->getOption("host");
	QString port = args->getOption("port");
	if (!host.isEmpty() && !port.isEmpty())
		m_atlantikNetwork->serverConnect(host, port.toInt());
	else
		showSelectServer();
}

void Atlantik::readConfig()
{
	// Read configuration settings

	// General configuration
	const KConfigGroup cggeneral(KGlobal::config(), "General");
	m_config.chatTimestamps = cggeneral.readEntry("ChatTimeStamps", false);

	// Personalization configuration
	const KConfigGroup cgpersonalization(KGlobal::config(), "Personalization");
	m_config.playerName = cgpersonalization.readEntry("PlayerName", "Atlantik");
	m_config.playerImage = cgpersonalization.readEntry("PlayerImage", "cube.png");

	// Board configuration
	const KConfigGroup cgboard(KGlobal::config(), "Board");
	m_config.indicateUnowned = cgboard.readEntry("IndicateUnowned", true);
	m_config.highliteUnowned = cgboard.readEntry("HighliteUnowned", false);
	m_config.darkenMortgaged = cgboard.readEntry("DarkenMortgaged", true);
	m_config.animateTokens = cgboard.readEntry("AnimateToken", false);
	m_config.quartzEffects = cgboard.readEntry("QuartzEffects", true);

	// Meta server configuation
	const KConfigGroup cgmonopigator(KGlobal::config(), "Monopigator");
	m_config.connectOnStart = cgmonopigator.readEntry("ConnectOnStart", false);
	m_config.hideDevelopmentServers = cgmonopigator.readEntry("HideDevelopmentServers", true);

	// Portfolio colors
	const KConfigGroup cgwm(KGlobal::config(), "WM");
	QColor activeDefault(204, 204, 204), inactiveDefault(153, 153, 153);
	m_config.activeColor = cgwm.readEntry("activeBackground", activeDefault);
	m_config.inactiveColor = cgwm.readEntry("inactiveBlend", inactiveDefault);
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

	KNotification::event("newplayer");
}

void Atlantik::newEstate(Estate *estate)
{
	initBoard();
	m_board->addEstateView(estate, m_config.indicateUnowned, m_config.highliteUnowned, m_config.darkenMortgaged, m_config.quartzEffects);
}

void Atlantik::newTrade(Trade *trade)
{
	TradeDisplay *tradeDisplay = new TradeDisplay(trade, m_atlanticCore);
        tradeDisplay->setObjectName("tradeDisplay");
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

	m_selectServer = new SelectServer(m_config.connectOnStart, m_config.hideDevelopmentServers, m_mainWidget );
        m_selectServer->setObjectName("selectServer");
	m_mainLayout->addWidget(m_selectServer, 0, 1, 3, 1);
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

	m_selectGame = new SelectGame(m_atlanticCore, m_mainWidget);
	m_atlanticCore->emitGames();

	m_mainLayout->addWidget(m_selectGame, 0, 1, 3, 1);
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

	m_selectConfiguration = new SelectConfiguration(m_atlanticCore, m_mainWidget );
        m_selectConfiguration->setObjectName("selectConfiguration");
	m_mainLayout->addWidget(m_selectConfiguration, 0, 1, 3, 1);
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

	m_board = new AtlantikBoard(m_atlanticCore, 40, AtlantikBoard::Play, m_mainWidget);
        m_board->setObjectName( "board" );
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

	m_mainLayout->addWidget(m_board, 0, 1, 3, 1);
	m_board->displayDefault();
	m_board->show();

	PortfolioView *portfolioView = 0;
	for (Q3PtrListIterator<PortfolioView> it(m_portfolioViews); *it; ++it)
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



void Atlantik::slotNetworkError(int /*errnum*/)
{
// 	QString errMsg(i18n("Error connecting: "));
//
// 	//Should be done by AtlantikNetwork...
// 	switch (m_atlantikNetwork->status())
// 	{
// 		case IO_ConnectError:
// 			if (errnum == ECONNREFUSED)
// 				errMsg.append(i18n("connection refused by host."));
// 			else
// 				errMsg.append(i18n("could not connect to host."));
// 			break;
//
// 		case IO_LookupError:
// 			errMsg.append(i18n("host not found."));
// 			break;
//
// 		default:
// 			errMsg.append(i18n("unknown error."));
// 	}
//
// 	serverMsgsAppend(errMsg);
//
// 	// Re-init network object
// 	initNetworkObject();
}

void Atlantik::networkClosed(int /*status*/)
{
// 	switch( status )
// 	{
// 	case KBufferedIO::involuntary:
// 		slotMsgStatus( i18n("Connection with server %1:%2 lost.").arg(m_atlantikNetwork->host()).arg(m_atlantikNetwork->port()), QString("connect_no") );
// 		showSelectServer();
// 		break;
// 	default:
// 		if ( !m_atlantikNetwork->host().isEmpty() )
// 			slotMsgStatus( i18n("Disconnected from %1:%2.").arg(m_atlantikNetwork->host()).arg(m_atlantikNetwork->port()), QString("connect_no") );
// 		break;
// 	}
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
	KNotifyConfigWidget::configure(this);
}

void Atlantik::slotUpdateConfig()
{
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

	KConfigGroup cggeneral(KGlobal::config(), "General");
	cggeneral.writeEntry("ChatTimeStamps", m_config.chatTimestamps);

	KConfigGroup cgpersonalization(KGlobal::config(), "Personalization");
	cgpersonalization.writeEntry("PlayerName", m_config.playerName);
	cgpersonalization.writeEntry("PlayerImage", m_config.playerImage);

	KConfigGroup cgboard(KGlobal::config(), "Board");
	cgboard.writeEntry("IndicateUnowned", m_config.indicateUnowned);
	cgboard.writeEntry("HighliteUnowned", m_config.highliteUnowned);
	cgboard.writeEntry("DarkenMortgaged", m_config.darkenMortgaged);
	cgboard.writeEntry("AnimateToken", m_config.animateTokens);
	cgboard.writeEntry("QuartzEffects", m_config.quartzEffects);

	KConfigGroup cgmonopigator(KGlobal::config(), "Monopigator");
	cgmonopigator.writeEntry("ConnectOnStart", m_config.connectOnStart);
	cgmonopigator.writeEntry("HideDevelopmentServers", m_config.hideDevelopmentServers);

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
	KXmlGuiWindow::statusBar()->changeItem(message, 1);
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
	KNotification::event("chat");
}

void Atlantik::serverMsgsAppend(QString msg)
{
	// Use append, not setText(old+new) because that one doesn't wrap
	m_serverMsgs->append(msg);
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
		for (Q3PtrListIterator<PortfolioView> it(m_portfolioViews); *it; ++it)
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
	KNotification::event("gainedturn", i18n("It is your turn now."), QPixmap() ,this );
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

void Atlantik::clientCookie(QString /*cookie*/)
{
// 	KSharedConfig::Ptr config = KGlobal::config();
//
// 	if (cookie.isNull())
// 	{
// 		if (config->hasGroup("Reconnection"))
// 			config->deleteGroup("Reconnection", true);
// 	}
// 	else if (m_atlantikNetwork)
// 	{
// 		config->setGroup("Reconnection");
// 		 config->writeEntry("Host", m_atlantikNetwork->host());
// 		 config->writeEntry("Port", m_atlantikNetwork->port());
// 		config->writeEntry("Cookie", cookie);
// 	}
// 	else
// 		return;
//
// 	config->sync();
}

void Atlantik::sendHandshake()
{
	m_atlantikNetwork->setName(m_config.playerName);
	m_atlantikNetwork->setImage(m_config.playerImage);

	// Check command-line args to see if we need to auto-join
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	QString game = args->getOption("game");
	kDebug() << "received Handshake; joining game: " << game.toInt() << endl;
	if (!game.isEmpty())
		m_atlantikNetwork->joinGame(game.toInt());
}

void Atlantik::statusBarClick(int item)
{
	if ( item == 0 )
	{
		KAboutApplicationDialog dialog(KGlobal::mainComponent().aboutData(), this);
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
	for (Q3PtrListIterator<PortfolioView> it(m_portfolioViews); (portfolioView = *it) ; ++it)
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
		result = KMessageBox::warningContinueCancel( this, i18n("You are currently part of an active game. Are you sure you want to close Atlantik? If you do, you forfeit the game."), i18n("Close & Forfeit?"), KGuiItem(i18n("Close && Forfeit")) );

	if ( result == KMessageBox::Continue )
	{
		if ( m_atlantikNetwork )
			m_atlantikNetwork->leaveGame();

                KConfigGroup cg( KGlobal::config(), "AtlantikMainWindow");
		saveMainWindowSettings( cg );
		KXmlGuiWindow::closeEvent(e);
	}
}
