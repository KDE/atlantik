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

#include <QLineEdit>
#include <qmenu.h>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QCloseEvent>
#include <QApplication>
#include <QScrollArea>
#include <QTextDocument>
#include <QCommandLineParser>
#include <QLocale>
#include <QStatusBar>

#include <kaboutapplicationdialog.h>
#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <knotifyconfigwidget.h>
#include <knotification.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <kgamestandardaction.h>
#else
#include <kstandardgameaction.h>
#endif
#include <kstandardaction.h>
#include <kiconloader.h>
#include <kaboutdata.h>

#include <atlantic_core.h>
#include <auction.h>
#include <estate.h>
#include <player.h>
#include <trade.h>
#include "atlantik.h"

#include <atlantik_network.h>

#include <board.h>
#include <trade_widget.h>
#include <portfolioview.h>

#include "eventlogwidget.h"
#include "version.h"
#include "selectserver_widget.h"
#include "selectgame_widget.h"
#include "selectconfiguration_widget.h"
#include "configdlg.h"
#include "clickablelabel.h"

#include <settings.h>
#include <atlantik_debug.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
namespace KStandardGameAction = KGameStandardAction;
#endif

LogTextEdit::LogTextEdit(QWidget *parent)
	: QTextEdit(parent)
{
	m_clear = KStandardAction::clear( this, SLOT( clear() ), nullptr );
	m_selectAll = KStandardAction::selectAll( this, SLOT( selectAll() ), nullptr );
	m_copy = KStandardAction::copy( this, SLOT( copy() ), nullptr );
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

Atlantik::Atlantik(QCommandLineParser *parser)
	: KXmlGuiWindow()
	, m_cliParser(parser)
	, m_atlantikNetwork(nullptr)
	, m_board(nullptr)
	, m_selectServer(nullptr)
	, m_selectGame(nullptr)
	, m_selectConfiguration(nullptr)
	, m_runningGame(false)
	, m_reconnecting(false)
	, m_tokenTheme(TokenTheme::defaultTheme())
{
	// Read application configuration
	readConfig();

	// Toolbar: Game
        m_showEventLog = actionCollection()->addAction(QStringLiteral("showeventlog"));
        m_showEventLog->setText(i18n("Show Event &Log..."));
		connect(m_showEventLog, SIGNAL(triggered(bool)), this, SLOT(showEventLog()));
	m_reconnect = actionCollection()->addAction(QStringLiteral("reconnect"));
	m_reconnect->setText(i18n("&Reconnect (After Crash)"));
	connect(m_reconnect, SIGNAL(triggered()), this, SLOT(slotReconnect()));
	m_reconnect->setEnabled(false);
	m_leaveGame = actionCollection()->addAction(QStringLiteral("leave_game"));
	m_leaveGame->setText(i18n("&Leave Game"));
	connect(m_leaveGame, SIGNAL(triggered()), this, SLOT(slotLeaveGame()));
	m_leaveGame->setEnabled(false);
	(void) KStandardGameAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());

	// Toolbar: Settings
	(void) KStandardAction::preferences(this, SLOT(slotConfigure()), actionCollection());
	(void) KStandardAction::configureNotifications(this, SLOT(configureNotifications()), actionCollection());

	// Game and network core
	m_atlanticCore = new AtlanticCore(this);
        m_atlanticCore->setObjectName(QStringLiteral("atlanticCore"));
	connect(m_atlanticCore, SIGNAL(createGUI(Player *)), this, SLOT(newPlayer(Player *)));
	connect(m_atlanticCore, SIGNAL(removeGUI(Player *)), this, SLOT(removeGUI(Player *)));
	connect(m_atlanticCore, SIGNAL(createGUI(Trade *)), this, SLOT(newTrade(Trade *)));
	connect(m_atlanticCore, SIGNAL(removeGUI(Trade *)), this, SLOT(removeGUI(Trade *)));

	m_eventLog = new EventLog(this);

	initNetworkObject();

	// Menu,toolbar: Move
	m_roll = KStandardGameAction::roll(this, SIGNAL(rollDice()), actionCollection());
	m_roll->setEnabled(false);

	m_buyEstate = actionCollection()->addAction(QStringLiteral("atlantik_buy_estate"));
        m_buyEstate->setText(i18n("&Buy"));
	KActionCollection::setDefaultShortcut(m_buyEstate, QKeySequence(Qt::CTRL | Qt::Key_B));
	m_buyEstate->setIcon(KDE::icon(QStringLiteral("atlantik_buy_estate")));
	connect (m_buyEstate,SIGNAL(triggered()), this, SIGNAL(buyEstate()));
	m_buyEstate->setEnabled(false);

	m_auctionEstate = actionCollection()->addAction(QStringLiteral("auction"));
        m_auctionEstate->setText(i18n("&Auction"));
	KActionCollection::setDefaultShortcut(m_auctionEstate, QKeySequence(Qt::CTRL | Qt::Key_A));
	m_auctionEstate->setIcon(KDE::icon(QStringLiteral("auction")));
	connect(m_auctionEstate,SIGNAL(triggered()),this, SIGNAL(auctionEstate()));
	m_auctionEstate->setEnabled(false);


	m_endTurn = KStandardGameAction::endTurn(this, SIGNAL(endTurn()), actionCollection());
	m_endTurn->setEnabled(false);

	m_jailCard = actionCollection()->addAction(QStringLiteral("move_jailcard"));
        m_jailCard->setText(i18n("Use Card to Leave Jail"));
	connect(m_jailCard, SIGNAL(triggered()),this, SIGNAL(jailCard()));
	m_jailCard->setEnabled(false);

	m_jailPay = actionCollection()->addAction(QStringLiteral("jail_pay"));
        m_jailPay->setText(i18n("&Pay to Leave Jail"));
	KActionCollection::setDefaultShortcut(m_jailPay, QKeySequence(Qt::CTRL | Qt::Key_P));
	m_jailPay->setIcon(KDE::icon(QStringLiteral("jail_pay")));
	connect(m_jailPay, SIGNAL(triggered()),this, SIGNAL(jailPay()));
	m_jailPay->setEnabled(false);

	m_jailRoll = actionCollection()->addAction(QStringLiteral("move_jailroll"));
        m_jailRoll->setText(i18n("Roll to Leave &Jail"));
	KActionCollection::setDefaultShortcut(m_jailRoll, QKeySequence(Qt::CTRL | Qt::Key_J));
	connect(m_jailRoll, SIGNAL(triggered()), this, SIGNAL(jailRoll()));
	m_jailRoll->setEnabled(false);

	// Mix code and XML into GUI
	setupGUI();
	m_sbVersion = new ClickableLabel();
	m_sbVersion->setText(QString::fromLatin1("Atlantik " ATLANTIK_VERSION_STRING));
	connect(m_sbVersion, SIGNAL(clicked()), this, SLOT(showAboutDialog()));
	statusBar()->addWidget(m_sbVersion, 0);
	m_sbStatus = new ClickableLabel();
	connect(m_sbStatus, SIGNAL(clicked()), this, SLOT(showEventLog()));
	statusBar()->addWidget(m_sbStatus, 1);

	// Main widget, containing all others
 	m_mainWidget = new QWidget(this);
        m_mainWidget->setObjectName(QStringLiteral("main"));
	m_mainWidget->show();
	m_mainLayout = new QGridLayout(m_mainWidget);
	setCentralWidget(m_mainWidget);

	// Vertical view area for portfolios.
	m_portfolioScroll = new QScrollArea(m_mainWidget);
	m_portfolioScroll->setObjectName(QStringLiteral("pfScroll"));
	m_mainLayout->addWidget( m_portfolioScroll, 0, 0 );
	m_portfolioScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_portfolioScroll->setWidgetResizable(true);
	m_portfolioScroll->setFixedHeight( 200 );
	m_portfolioScroll->hide();

	m_portfolioWidget = new QWidget();
        m_portfolioWidget->setObjectName(QStringLiteral("pfWidget"));
	m_portfolioScroll->setWidget(m_portfolioWidget);
	m_portfolioWidget->show();

	m_portfolioLayout = new QVBoxLayout(m_portfolioWidget);
	m_portfolioLayout->setContentsMargins(0, 0, 0, 0);

	// Nice label
//	m_portfolioLabel = new QLabel(i18n("Players"), m_portfolioWidget, "pfLabel");
//	m_portfolioLayout->addWidget(m_portfolioLabel);
//	m_portfolioLabel->show();

	// Text view for chat and status messages from server.
	m_serverMsgs = new LogTextEdit(m_mainWidget);
	m_serverMsgs->setObjectName(QLatin1String("serverMsgs"));
	m_serverMsgs->setReadOnly(true);
	m_serverMsgs->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_serverMsgs->setMinimumWidth(200);
	m_serverMsgs->setUndoRedoEnabled(false);
	m_mainLayout->addWidget(m_serverMsgs, 1, 0);

	// LineEdit to enter commands and chat messages.
	m_input = new QLineEdit(m_mainWidget);
	m_input->setObjectName(QStringLiteral("input"));
	m_mainLayout->addWidget(m_input, 2, 0);

	m_serverMsgs->setFocusProxy(m_input);

	connect(m_input, SIGNAL(returnPressed()), this, SLOT(slotSendMsg()));

	// Set stretching where we want it.
	m_mainLayout->setRowStretch(1, 1); // make m_board+m_serverMsgs stretch vertically, not the rest
	m_mainLayout->setColumnStretch(1, 1); // make m_board stretch horizontally, not the rest

	// Check command-line args to see if we need to connect or show the metaserver window
	QString host = m_cliParser ? m_cliParser->value(QStringLiteral("host")) : QString();
	QString port = m_cliParser ? m_cliParser->value(QStringLiteral("port")) : QString();
	if (!host.isEmpty() && !port.isEmpty())
		m_atlantikNetwork->serverConnect(host, port.toInt());
	else
		showSelectServer();

	// Connection cookie
	m_reconnectCookie.reset(ConnectionCookie::read());
	m_reconnect->setEnabled(m_reconnectCookie != nullptr);
}

void Atlantik::readConfig()
{
	// Read configuration settings

	// General configuration
	m_config.chatTimestamps = Settings::chatTimeStamps();

	// Personalization configuration
	m_config.playerName = Settings::playerName();
	m_config.playerImage = Settings::playerImage();

	// Board configuration
	m_config.indicateUnowned = Settings::indicateUnowned();
	m_config.highlightUnowned = Settings::highlightUnowned();
	m_config.darkenMortgaged = Settings::darkenMortgaged();
	m_config.animateTokens = Settings::animateToken();
	m_config.quartzEffects = Settings::quartzEffects();

	// Meta server configuation
	m_config.connectOnStart = Settings::connectOnStart();
	m_config.hideDevelopmentServers = Settings::hideDevelopmentServers();

	// Portfolio colors
	m_config.activeColor = Settings::activeBackground();
	m_config.inactiveColor = Settings::inactiveBlend();
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

	if (!player->isSelf())
		KNotification::event(QStringLiteral("newplayer"), i18n("New player joined."));
}

void Atlantik::newEstate(Estate *estate)
{
	initBoard();
	m_board->addEstateView(estate, m_config.indicateUnowned, m_config.highlightUnowned, m_config.darkenMortgaged, m_config.quartzEffects);
}

void Atlantik::newTrade(Trade *trade)
{
	TradeDisplay *tradeDisplay = new TradeDisplay(trade, m_atlanticCore);
        tradeDisplay->setObjectName(QStringLiteral("tradeDisplay"));
	m_tradeGUIMap.insert(trade, tradeDisplay);
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
	{
		m_portfolioViews.removeOne(portfolioView);
		delete portfolioView;
	}

	if (m_board)
		m_board->removeToken(player);
}

void Atlantik::removeGUI(Trade *trade)
{
	delete m_tradeGUIMap.take(trade);
}

void Atlantik::showSelectServer()
{
	if (m_selectServer)
		return;

	m_selectServer = new SelectServer(m_config.hideDevelopmentServers, m_mainWidget );
        m_selectServer->setObjectName(QStringLiteral("selectServer"));
	m_mainLayout->addWidget(m_selectServer, 0, 1, 3, 1);
	m_selectServer->show();

	if (m_selectGame)
	{
		delete m_selectGame;
		m_selectGame = nullptr;
	}

	m_atlanticCore->reset(true);
	initNetworkObject();

	m_reconnect->setEnabled(false);
	m_reconnecting = false;
	m_leaveGame->setEnabled(false);

	connect(m_selectServer, SIGNAL(serverConnect(QString, int)), m_atlantikNetwork, SLOT(serverConnect(QString, int)));
	connect(m_selectServer, SIGNAL(msgStatus(const QString &)), this, SLOT(slotMsgStatus(const QString &)));

	if (m_config.connectOnStart)
		m_selectServer->reloadServerList();
}

void Atlantik::showSelectGame()
{
	if (m_selectGame)
		return;

	m_selectGame = new SelectGame(m_atlanticCore, m_mainWidget);
	m_atlanticCore->emitGames();
	m_leaveGame->setEnabled(false);

	m_mainLayout->addWidget(m_selectGame, 0, 1, 3, 1);
	m_selectGame->show();

	// Reset core and GUI
	if (m_board)
	{
		m_board->hide();
		m_board->reset();
//		delete m_board;
//		m_board = nullptr;

		// m_portfolioViews.clear();
		m_atlanticCore->reset();
	}

	if (m_selectServer)
	{
		delete m_selectServer;
		m_selectServer = nullptr;
	}
	if (m_selectConfiguration)
	{
		delete m_selectConfiguration;
		m_selectConfiguration = nullptr;
	}

	connect(m_selectGame, SIGNAL(joinGame(int)), m_atlantikNetwork, SLOT(joinGame(int)));
	connect(m_selectGame, SIGNAL(watchGame(int)), m_atlantikNetwork, SLOT(watchGame(int)));
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
		m_selectGame = nullptr;
	}

	m_selectConfiguration = new SelectConfiguration(m_atlanticCore, m_mainWidget );
        m_selectConfiguration->setObjectName(QStringLiteral("selectConfiguration"));
	m_mainLayout->addWidget(m_selectConfiguration, 0, 1, 3, 1);
	m_selectConfiguration->show();
	m_leaveGame->setEnabled(true);

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
        m_board->setObjectName(QStringLiteral("board"));
	m_board->setViewProperties(m_config.indicateUnowned, m_config.highlightUnowned, m_config.darkenMortgaged, m_config.quartzEffects, m_config.animateTokens);
	m_board->setTokenTheme(m_tokenTheme);

	connect(m_atlantikNetwork, SIGNAL(displayDetails(QString, bool, bool, Estate *)), m_board, SLOT(insertDetails(QString, bool, bool, Estate *)));
	connect(m_atlantikNetwork, SIGNAL(displayText(QString, bool, bool)), m_board, SLOT(insertText(QString, bool, bool)));
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
		m_selectGame = nullptr;
	}

	if (m_selectConfiguration)
	{
		delete m_selectConfiguration;
		m_selectConfiguration = nullptr;
	}

	if (!m_board)
		initBoard();

	m_runningGame = true;
	m_leaveGame->setEnabled(true);

	m_mainLayout->addWidget(m_board, 0, 1, 3, 1);
	m_board->displayDefault();
	m_board->show();

	foreach (PortfolioView *portfolioView, m_portfolioViews)
		if (!portfolioView->player()->isSpectator())
			portfolioView->buildPortfolio();
}

void Atlantik::freezeBoard()
{
	if (!m_board)
		showBoard();

	m_runningGame = false;
	m_leaveGame->setEnabled(false);
	// TODO: m_board->freeze();
}

void Atlantik::slotNetworkConnected()
{
}



void Atlantik::slotNetworkError(QAbstractSocket::SocketError errnum)
{
	switch (errnum) {
	case QAbstractSocket::ConnectionRefusedError:
		appendMsg(i18n("Error connecting: connection refused by host."), ErrorMsg);
		break;
	case QAbstractSocket::HostNotFoundError:
		appendMsg(i18n("Error connecting: host not found."), ErrorMsg);
		break;
	default:
		appendMsg(i18n("Error connecting: error %1.", QString::number(errnum)), ErrorMsg);
		break;
	}

	// Re-init network object
	initNetworkObject();
}

void Atlantik::slotNetworkDisconnected()
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
	if (KConfigDialog::showDialog(QStringLiteral("configdialog")))
		return;

	ConfigDialog *dialog = new ConfigDialog(m_tokenTheme, this);
	connect(dialog, SIGNAL(settingsChanged(QString)), this, SLOT(slotUpdateConfig()));

	dialog->show();
}

void Atlantik::showEventLog()
{
	bool created = false;
	if (!m_eventLogWidget)
	{
		m_eventLogWidget = new EventLogWidget(m_eventLog, this);
		created = true;
	}
	m_eventLogWidget->show();
	if (created)
		m_eventLogWidget->restoreSettings();
}

void Atlantik::configureNotifications()
{
	KNotifyConfigWidget::configure(this);
}

void Atlantik::slotUpdateConfig()
{
	bool optBool, viewConfigChanged = false;
	QString optStr;

	optBool = Settings::chatTimeStamps();
	if (m_config.chatTimestamps != optBool)
	{
		m_config.chatTimestamps = optBool;
	}

	optStr = Settings::playerName();
	if (m_config.playerName != optStr)
	{
		m_config.playerName = optStr;
		if (m_atlantikNetwork->isConnected())
			m_atlantikNetwork->setName(optStr);
	}

	optStr = Settings::playerImage();
	if (m_config.playerImage != optStr)
	{
		m_config.playerImage = optStr;
		if (m_atlantikNetwork->isConnected())
			m_atlantikNetwork->setImage(optStr);
	}

	optBool = Settings::indicateUnowned();
	if (m_config.indicateUnowned != optBool)
	{
		m_config.indicateUnowned = optBool;
		viewConfigChanged = true;
	}

	optBool = Settings::highlightUnowned();
	if (m_config.highlightUnowned != optBool)
	{
		m_config.highlightUnowned = optBool;
		viewConfigChanged = true;
	}

	optBool = Settings::darkenMortgaged();
	if (m_config.darkenMortgaged != optBool)
	{
		m_config.darkenMortgaged = optBool;
		viewConfigChanged = true;
	}

	optBool = Settings::animateToken();
	if (m_config.animateTokens != optBool)
	{
		m_config.animateTokens = optBool;
		viewConfigChanged = true;
	}

	optBool = Settings::quartzEffects();
	if (m_config.quartzEffects != optBool)
	{
		m_config.quartzEffects = optBool;
		viewConfigChanged = true;
	}

	optBool = Settings::connectOnStart();
	if (m_config.connectOnStart != optBool)
	{
		m_config.connectOnStart = optBool;
	}

	optBool = Settings::hideDevelopmentServers();
	if (m_config.hideDevelopmentServers != optBool)
	{
		m_config.hideDevelopmentServers = optBool;
		if (m_selectServer)
			m_selectServer->setHideDevelopmentServers(optBool);
	}

	if (viewConfigChanged && m_board)
		m_board->setViewProperties(m_config.indicateUnowned, m_config.highlightUnowned, m_config.darkenMortgaged, m_config.quartzEffects, m_config.animateTokens);
}

void Atlantik::slotSendMsg()
{
	if (m_atlantikNetwork->isConnected())
		m_atlantikNetwork->cmdChat(m_input->text());
	m_input->setText(QString());
}

void Atlantik::slotMsgInfo(const QString &msg)
{
	appendMsg(msg, InfoMsg);
}

void Atlantik::slotMsgError(const QString &msg)
{
	appendMsg(msg, ErrorMsg);
}

void Atlantik::slotMsgStatus(const QString &message, EventType type)
{
	m_sbStatus->setText(message);
	m_eventLog->addEvent(message, type);
}

static bool commandForMe(const QList<QStringView> &parts, const QString &playerName)
{
	if (parts.size() <= 1)
		return true;

	foreach (const QStringView &p, parts)
	{
		if (p == playerName)
			return true;
	}

	return false;
}

void Atlantik::slotMsgChat(const QString &player, const QString &msg)
{
	QString res;
	if (msg == QLatin1String("/me"))
		res = QStringLiteral("* %1").arg(player);
	else if (msg.startsWith(QLatin1String("/me ")))
		res = QStringLiteral("* %1 %2").arg(player, msg.mid(4));
	else if (msg.startsWith(QLatin1String("[ACTION] ")))
		res = QStringLiteral("* %1 %2").arg(player, msg.mid(9));
	else
		res = QStringLiteral("<%1> %2").arg(player, msg);
	appendMsg(res, ChatMsg);
	Player *playerSelf = m_atlanticCore->playerSelf();
	if (!isActiveWindow() && (!playerSelf || playerSelf->name() != player))
		KNotification::event(QStringLiteral("chat"), QStringLiteral("%1: %2").arg(player, msg.toHtmlEscaped()));
	if (m_atlantikNetwork->isConnected() && playerSelf && msg.startsWith(QLatin1Char('!')))
	{
		const QList<QStringView> parts = QStringView(msg).split(QLatin1Char(' '), Qt::SkipEmptyParts);
		Q_ASSERT(!parts.isEmpty());
		if (commandForMe(parts, playerSelf->name()))
		{
			const QStringView cmd = parts.first();
			if (cmd == QLatin1String("!date"))
				m_atlantikNetwork->cmdChat(QLocale::c().toString(QDateTime::currentDateTime(), QLocale::ShortFormat));
			else if (cmd == QLatin1String("!ping"))
				m_atlantikNetwork->cmdChat(QStringLiteral("pong"));
			else if (cmd == QLatin1String("!version"))
				m_atlantikNetwork->cmdChat(QString::fromLatin1("Atlantik " ATLANTIK_VERSION_STRING));
		}
	}
}

void Atlantik::appendMsg(const QString &msg, MsgType type)
{
	const QString escaped = msg.toHtmlEscaped();
	QString ts;
	QString res;

	if (m_config.chatTimestamps)
	{
		const QString timeString = QLocale::system().toString(QTime::currentTime(), QLocale::ShortFormat);
		ts = QStringLiteral("[%1] ").arg(timeString);
	}

	switch (type)
	{
	case ErrorMsg:
		res = QStringLiteral("<font color=\"%1\">%2[%3] %4</font>").arg(QStringLiteral("#ff0000"), ts, i18nc("error message", "Error"), escaped);
		break;
	case InfoMsg:
		res = QStringLiteral("<font color=\"%1\">%2[%3] %4</font>").arg(QStringLiteral("#91640a"), ts, i18nc("informative message", "Info"), escaped);
		break;
	case ChatMsg:
		res = QStringLiteral("<font color=\"%1\">%2</font>%3").arg(QStringLiteral("#709070"), ts, escaped);
		break;
	}
	res += QStringLiteral("<br/>\n");

	m_serverMsgs->insertHtml(res);
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
		foreach (PortfolioView *portfolioView, m_portfolioViews)
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
	KNotification::event(QStringLiteral("gainedturn"), i18n("It is your turn now."), QPixmap());
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
	connect(m_atlantikNetwork, SIGNAL(msgStatus(const QString &, EventType)), this, SLOT(slotMsgStatus(const QString &, EventType)));
	connect(m_atlantikNetwork, SIGNAL(msgChat(QString, QString)), this, SLOT(slotMsgChat(QString, QString)));

	connect(m_atlantikNetwork, SIGNAL(connectionSuccess()), this, SLOT(slotNetworkConnected()));
	connect(m_atlantikNetwork, SIGNAL(connectionFailed(QAbstractSocket::SocketError)), this, SLOT(slotNetworkError(QAbstractSocket::SocketError)));
	connect(m_atlantikNetwork, SIGNAL(disconnected()), this, SLOT(slotNetworkDisconnected()));

	connect(m_atlantikNetwork, SIGNAL(receivedHandshake()), this, SLOT(sendHandshake()));

	connect(m_atlantikNetwork, SIGNAL(gameConfig()), this, SLOT(showSelectConfiguration()));
	connect(m_atlantikNetwork, SIGNAL(gameInit()), this, SLOT(initBoard()));
	connect(m_atlantikNetwork, SIGNAL(gameRun()), this, SLOT(showBoard()));
	connect(m_atlantikNetwork, SIGNAL(gameEnd()), this, SLOT(freezeBoard()));

	connect(m_atlantikNetwork, SIGNAL(newEstate(Estate *)), this, SLOT(newEstate(Estate *)));
	connect(m_atlantikNetwork, SIGNAL(newAuction(Auction *)), this, SLOT(newAuction(Auction *)));

	connect(m_atlantikNetwork, SIGNAL(clientCookie(QString)), this, SLOT(clientCookie(QString)));
	connect(m_atlantikNetwork, SIGNAL(networkEvent(const QString &, EventType)), m_eventLog, SLOT(addEvent(const QString &, EventType)));

	connect(this, SIGNAL(rollDice()), m_atlantikNetwork, SLOT(rollDice()));
	connect(this, SIGNAL(buyEstate()), m_atlantikNetwork, SLOT(buyEstate()));
	connect(this, SIGNAL(auctionEstate()), m_atlantikNetwork, SLOT(auctionEstate()));
	connect(this, SIGNAL(endTurn()), m_atlantikNetwork, SLOT(endTurn()));
	connect(this, SIGNAL(jailCard()), m_atlantikNetwork, SLOT(jailCard()));
	connect(this, SIGNAL(jailPay()), m_atlantikNetwork, SLOT(jailPay()));
	connect(this, SIGNAL(jailRoll()), m_atlantikNetwork, SLOT(jailRoll()));
	connect(this, SIGNAL(reconnect(QString)), m_atlantikNetwork, SLOT(reconnect(QString)));
}

void Atlantik::clientCookie(const QString &cookie)
{
	ConnectionCookie *newCookie = nullptr;

	if (!cookie.isEmpty() && m_atlantikNetwork)
		newCookie = new ConnectionCookie(m_atlantikNetwork->host(), m_atlantikNetwork->port(), cookie);

	m_cookie.reset(newCookie);
	m_reconnect->setEnabled(false);
}

void Atlantik::sendHandshake()
{
	if (m_reconnecting)
	{
		m_reconnecting = false;
		Q_EMIT reconnect(m_reconnectCookie->cookie());
		m_reconnectCookie.reset();
		return;
	}

	m_atlantikNetwork->setName(m_config.playerName);
	m_atlantikNetwork->setImage(m_config.playerImage);

	// Check command-line args to see if we need to auto-join
	QString game = m_cliParser ? m_cliParser->value(QStringLiteral("game")) : QString();
	qCDebug(ATLANTIK_LOG) << "received Handshake; joining game:" << game.toInt();
	if (!game.isEmpty())
		m_atlantikNetwork->joinGame(game.toInt());
}

void Atlantik::showAboutDialog()
{
	KAboutApplicationDialog dialog(KAboutData::applicationData(), this);
	dialog.exec();
}

PortfolioView *Atlantik::addPortfolioView(Player *player)
{
	PortfolioView *portfolioView = new PortfolioView(m_atlanticCore, player, m_config.activeColor, m_config.inactiveColor, m_portfolioWidget);
	portfolioView->setTokenTheme(m_tokenTheme);
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
	foreach (PortfolioView *portfolioView, m_portfolioViews)
		if (player == portfolioView->player())
			return portfolioView;

	return nullptr;
}

void Atlantik::closeEvent(QCloseEvent *e)
{
	Game *gameSelf = m_atlanticCore->gameSelf();
	Player *playerSelf = m_atlanticCore->playerSelf();

	int result = KMessageBox::Continue;
	if ( gameSelf && !playerSelf->isBankrupt() && m_runningGame && !playerSelf->isSpectator() )
		result = KMessageBox::warningContinueCancel( this, i18n("You are currently part of an active game. Are you sure you want to close Atlantik? If you do, you forfeit the game."), i18n("Close & Forfeit?"), KGuiItem(i18n("Close && Forfeit")) );

	if ( result == KMessageBox::Continue )
	{
		if ( m_atlantikNetwork && m_atlantikNetwork->isConnected() )
		{
			m_atlantikNetwork->leaveGame();
			m_atlantikNetwork->disconnect();
		}

		KXmlGuiWindow::closeEvent(e);
	}
	else
		e->ignore();
}

void Atlantik::slotReconnect()
{
	if (!m_reconnectCookie)
		return;

	m_atlantikNetwork->serverConnect(m_reconnectCookie->host(), m_reconnectCookie->port());
	m_reconnecting = true;
}

void Atlantik::slotLeaveGame()
{
	Game *gameSelf = m_atlanticCore->gameSelf();
	Player *playerSelf = m_atlanticCore->playerSelf();

	if (!gameSelf)
		return;

	if (!m_runningGame || playerSelf->isBankrupt() || playerSelf->isSpectator() || (KMessageBox::warningContinueCancel(this, i18n("You are currently part of an active game. Are you sure you want to leave it? If you do, you forfeit the game."), i18n("Leave & Forfeit?"), KGuiItem(i18n("Leave && Forfeit"))) == KMessageBox::Continue))
		m_atlantikNetwork->leaveGame();
}

#include "moc_atlantik.cpp"
