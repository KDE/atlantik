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
#include "selectserver_widget.h"
#include "selectgame_widget.h"
#include "selectconfiguration_widget.h"

#include "trade_widget.h"

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

	// Initialize pointers to 0L
	m_configDialog = 0;

	// Network layer
	m_gameNetwork = new GameNetwork(this, "gameNetwork");

	connect(m_gameNetwork, SIGNAL(msgError(QString)), this, SLOT(slotMsgError(QString)));
	connect(m_gameNetwork, SIGNAL(msgChat(QString, QString)), this, SLOT(slotMsgChat(QString, QString)));
	connect(m_gameNetwork, SIGNAL(msgStartGame(QString)), this, SLOT(slotMsgStartGame(QString)));
	connect(m_gameNetwork, SIGNAL(msgPlayerUpdateName(int, QString)), this, SLOT(slotMsgPlayerUpdateName(int, QString)));
	connect(m_gameNetwork, SIGNAL(msgPlayerUpdateMoney(int, QString)), this, SLOT(slotMsgPlayerUpdateMoney(int, QString)));
	connect(m_gameNetwork, SIGNAL(msgPlayerUpdateJailed(int, bool)), this, SLOT(slotMsgPlayerUpdateJailed(int, bool)));
	connect(m_gameNetwork, SIGNAL(msgEstateUpdateOwner(int, int)), this, SLOT(slotMsgEstateUpdateOwner(int, int)));
	connect(m_gameNetwork, SIGNAL(msgEstateUpdateName(int, QString)), this, SLOT(slotMsgEstateUpdateName(int, QString)));
	connect(m_gameNetwork, SIGNAL(msgEstateUpdateColor(int, QString)), this, SLOT(slotMsgEstateUpdateColor(int, QString)));
	connect(m_gameNetwork, SIGNAL(msgEstateUpdateBgColor(int, QString)), this, SLOT(slotMsgEstateUpdateBgColor(int, QString)));
	connect(m_gameNetwork, SIGNAL(msgEstateUpdateHouses(int, int)), this, SLOT(slotMsgEstateUpdateHouses(int, int)));
	connect(m_gameNetwork, SIGNAL(msgEstateUpdateGroupId(int, int)), this, SLOT(slotMsgEstateUpdateGroupId(int, int)));
	connect(m_gameNetwork, SIGNAL(msgEstateUpdateMortgaged(int, bool)), this, SLOT(slotMsgEstateUpdateMortgaged(int, bool)));
	connect(m_gameNetwork, SIGNAL(msgEstateUpdateCanToggleMortgage(int, bool)), this, SLOT(slotMsgEstateUpdateCanToggleMortgage(int, bool)));
	connect(m_gameNetwork, SIGNAL(msgEstateUpdateCanBeOwned(int, bool)), this, SLOT(slotMsgEstateUpdateCanBeOwned(int, bool)));
	connect(m_gameNetwork, SIGNAL(estateUpdateCanBuyHouses(int, bool)), this, SLOT(slotEstateUpdateCanBuyHouses(int, bool)));
	connect(m_gameNetwork, SIGNAL(estateUpdateCanSellHouses(int, bool)), this, SLOT(slotEstateUpdateCanSellHouses(int, bool)));
	connect(m_gameNetwork, SIGNAL(estateUpdateFinished(int)), this, SLOT(slotEstateUpdateFinished(int)));
	connect(m_gameNetwork, SIGNAL(playerUpdateFinished(int)), this, SLOT(slotPlayerUpdateFinished(int)));
	connect(m_gameNetwork, SIGNAL(setPlayerId(int)), this, SLOT(slotSetPlayerId(int)));
	connect(m_gameNetwork, SIGNAL(setTurn(int)), this, SLOT(slotSetTurn(int)));
	connect(m_gameNetwork, SIGNAL(tradeUpdatePlayerAdd(int, int)), this, SLOT(slotTradeUpdatePlayerAdd(int, int)));
	connect(m_gameNetwork, SIGNAL(tradeUpdateEstate(int, int, int)), this, SLOT(slotTradeUpdateEstate(int, int, int)));
	connect(m_gameNetwork, SIGNAL(tradeUpdateMoney(int, int, int, unsigned int)), this, SLOT(slotTradeUpdateMoney(int, int, int, unsigned int)));

	connect(m_gameNetwork, SIGNAL(connected()), this, SLOT(slotNetworkConnected()));
	connect(m_gameNetwork, SIGNAL(error(int)), this, SLOT(slotNetworkError(int)));
	connect(m_gameNetwork, SIGNAL(joinedGame()), this, SLOT(slotJoinedGame()));
	connect(m_gameNetwork, SIGNAL(initGame()), this, SLOT(slotInitGame()));

	// Toolbar: Move
	m_roll = KStdGameAction::roll(m_gameNetwork, SLOT(roll()), actionCollection()); // No Ctrl-R at the moment
	m_roll->setEnabled(false);
	m_buyEstate = new KAction("&Buy", "atlantik_buy_estate", CTRL+Key_B, m_gameNetwork, SLOT(buyEstate()), actionCollection(), "buy_estate");
	m_buyEstate->setEnabled(false);
	m_endTurn = KStdGameAction::endTurn(m_gameNetwork, SLOT(endTurn()), actionCollection());
	m_endTurn->setEnabled(false);
	m_jailCard = new KAction("Use card to leave jail", "altantik_move_jail_card", 0, m_gameNetwork, SLOT(jailCard()), actionCollection(), "move_jailcard");
	m_jailCard->setEnabled(false);
	m_jailPay = new KAction("&Pay to leave jail", "altantik_move_jail_pay", CTRL+Key_P, m_gameNetwork, SLOT(jailPay()), actionCollection(), "move_jailpay");
	m_jailPay->setEnabled(false);
	m_jailRoll = new KAction("&Roll to leave jail", "altantik_move_jail_roll", CTRL+Key_R, m_gameNetwork, SLOT(jailRoll()), actionCollection(), "move_jailroll");
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

	connect(m_gameNetwork, SIGNAL(playerInit(int)), this, SLOT(slotPlayerInit(int)));
	connect(m_gameNetwork, SIGNAL(estateInit(int)), this, SLOT(slotEstateInit(int)));
	connect(m_gameNetwork, SIGNAL(tradeInit(int)), this, SLOT(slotTradeInit(int)));

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

	connect(m_selectServer, SIGNAL(serverConnect(const QString, int)), m_gameNetwork, SLOT(serverConnect(const QString, int)));

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
	atlantikConfig.grayOutMortgaged = config->readBoolEntry("GrayOutMortgaged", true);
	atlantikConfig.animateToken = config->readBoolEntry("AnimateToken", false);
	atlantikConfig.quartzEffects = config->readBoolEntry("QuartzEffects", true);
}

void Atlantik::slotNetworkConnected()
{
	// We're connected, so let's make ourselves known.
	m_gameNetwork->cmdName(atlantikConfig.playerName);

	// Create select game widget and replace the select server widget.
	m_selectGame = new SelectGame(m_mainWidget, "selectGame");
	m_mainLayout->addMultiCellWidget(m_selectGame, 0, 2, 1, 1);
	m_selectGame->show();
	if (m_selectServer)
	{
		delete m_selectServer;
		m_selectServer = 0;
	}

	connect(m_gameNetwork, SIGNAL(gameListClear()), m_selectGame, SLOT(slotGameListClear()));
	connect(m_gameNetwork, SIGNAL(gameListAdd(QString, QString, QString, QString)), m_selectGame, SLOT(slotGameListAdd(QString, QString, QString, QString)));
	connect(m_gameNetwork, SIGNAL(gameListEdit(QString, QString, QString, QString)), m_selectGame, SLOT(slotGameListEdit(QString, QString, QString, QString)));
	connect(m_gameNetwork, SIGNAL(gameListDel(QString)), m_selectGame, SLOT(slotGameListDel(QString)));

	connect(m_selectGame, SIGNAL(joinGame(int)), m_gameNetwork, SLOT(joinGame(int)));
	connect(m_selectGame, SIGNAL(newGame(const QString &)), m_gameNetwork, SLOT(newGame(const QString &)));
}

void Atlantik::slotNetworkError(int errno)
{
	QString errMsg(i18n("Error connecting: "));
	
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

	connect(m_gameNetwork, SIGNAL(playerListClear()), m_selectConfiguration, SLOT(slotPlayerListClear()));
	connect(m_gameNetwork, SIGNAL(playerListAdd(QString, QString, QString)), m_selectConfiguration, SLOT(slotPlayerListAdd(QString, QString, QString)));
	connect(m_gameNetwork, SIGNAL(playerListEdit(QString, QString, QString)), m_selectConfiguration, SLOT(slotPlayerListEdit(QString, QString, QString)));
	connect(m_gameNetwork, SIGNAL(playerListDel(QString)), m_selectConfiguration, SLOT(slotPlayerListDel(QString)));

	connect(m_selectConfiguration, SIGNAL(startGame()), m_gameNetwork, SLOT(startGame()));
}

void Atlantik::slotInitGame()
{
	// Create board widget and replace the game configuration widget.
	m_board = new AtlantikBoard(m_mainWidget, "board");
	m_mainLayout->addMultiCellWidget(m_board, 0, 2, 1, 1);
	m_board->show();
	if (m_selectConfiguration)
	{
		delete m_selectConfiguration;
		m_selectConfiguration = 0;
	}

	connect(m_gameNetwork, SIGNAL(msgPlayerUpdateLocation(int, int, bool)), this, SLOT(slotMsgPlayerUpdateLocation(int, int, bool)));
	connect(m_gameNetwork, SIGNAL(msgPlayerUpdateLocation(int, int, bool)), m_board, SLOT(slotMsgPlayerUpdateLocation(int, int, bool)));
	connect(m_gameNetwork, SIGNAL(displayCard(QString, QString)), m_board, SLOT(slotDisplayCard(QString, QString)));
	connect(m_board, SIGNAL(tokenConfirmation(int)), m_gameNetwork, SLOT(cmdTokenConfirmation(int)));
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
		m_gameNetwork->cmdName(optStr);
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

	optBool = m_configDialog->grayOutMortgaged();
	if (atlantikConfig.grayOutMortgaged != optBool)
	{
		atlantikConfig.grayOutMortgaged = optBool;
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
	config->writeEntry("GrayOutMortgaged", atlantikConfig.grayOutMortgaged);
	config->writeEntry("AnimateToken", atlantikConfig.animateToken);
	config->writeEntry("QuartzEffects", atlantikConfig.quartzEffects);

	config->sync();

	if (redrawEstates)
	{
		for(unsigned int estateId=0; estateId < estateMap.size() ; estateId++)
		{
			Estate *estate = estateMap[estateId];
			estate->update(true);
		}
	}
}

void Atlantik::slotSendMsg()
{
	m_gameNetwork->cmdChat(m_input->text());
	m_input->setText("");
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

void Atlantik::slotMsgPlayerUpdateLocation(int playerId, int estateId, bool /*directMove*/)
{
	kdDebug() << "Atlantik::slotMsgPlayerUpdateLocation" << endl;
	Player *player = playerMap[playerId];
	if (player)
		player->setLocation(estateId);
}

void Atlantik::slotMsgPlayerUpdateName(int playerId, QString name)
{
	Player *player = playerMap[playerId];
	if (player)
		player->setName(name);
}

void Atlantik::slotMsgPlayerUpdateMoney(int playerId, QString money)
{
	Player *player = playerMap[playerId];
	if (player)
		player->setMoney("$ " + money);
}

void Atlantik::slotMsgPlayerUpdateJailed(int playerId, bool inJail)
{
	Player *player = playerMap[playerId];
	if (player)
		player->setInJail(inJail);
}

void Atlantik::slotMsgEstateUpdateOwner(int estateId, int playerId)
{
	Estate *estate = estateMap[estateId];
	Player *player = playerMap[playerId];

	if (estate)
		estate->setOwner(player);
}

void Atlantik::slotMsgEstateUpdateName(int estateId, QString name)
{
	if (Estate *estate = estateMap[estateId])
		estate->setName(name);
}

void Atlantik::slotMsgEstateUpdateColor(int estateId, QString colorStr)
{
	QColor color;
	kdDebug() << "setting color " << colorStr << endl;
	color.setNamedColor(colorStr);
	
	if (Estate *estate = estateMap[estateId])
		estate->setColor(color);
}

void Atlantik::slotMsgEstateUpdateBgColor(int estateId, QString colorStr)
{
	QColor color;
	color.setNamedColor(colorStr);

	if (Estate *estate = estateMap[estateId])
		estate->setBgColor(color);
}

void Atlantik::slotMsgEstateUpdateHouses(int estateId, int houses)
{
	if (Estate *estate = estateMap[estateId])
		estate->setHouses(houses);
}

void Atlantik::slotMsgEstateUpdateGroupId(int estateId, int groupId)
{
	if (Estate *estate = estateMap[estateId])
		estate->setGroupId(groupId);
}

void Atlantik::slotMsgEstateUpdateMortgaged(int estateId, bool mortgaged)
{
	if (Estate *estate = estateMap[estateId])
		estate->setIsMortgaged(mortgaged);
}

void Atlantik::slotMsgEstateUpdateCanToggleMortgage(int estateId, bool canToggleMortgage)
{
	if (Estate *estate = estateMap[estateId])
		estate->setCanToggleMortgage(canToggleMortgage);
}

void Atlantik::slotMsgEstateUpdateCanBeOwned(int estateId, bool canBeOwned)
{
	if (Estate *estate = estateMap[estateId])
		estate->setCanBeOwned(canBeOwned);
}

void Atlantik::slotEstateUpdateCanBuyHouses(int estateId, bool canBuyHouses)
{
	if (Estate *estate = estateMap[estateId])
		estate->setCanBuyHouses(canBuyHouses);
}


void Atlantik::slotEstateUpdateCanSellHouses(int estateId, bool canSellHouses)
{
	if (Estate *estate = estateMap[estateId])
		estate->setCanSellHouses(canSellHouses);
}

void Atlantik::slotEstateUpdateFinished(int estateId)
{
	if (Estate *estate = estateMap[estateId])
		estate->update();
}

void Atlantik::slotPlayerUpdateFinished(int playerId)
{
	if (Player *player = playerMap[playerId])
		player->update();
}

void Atlantik::slotSetPlayerId(int playerId)
{
	if (Player *player = playerMap[playerId])
		player->setIsSelf(true);
}

void Atlantik::slotSetTurn(int playerId)
{
	Player *player = playerMap[playerId];
	if (player && player->isSelf())
	{
		m_roll->setEnabled(true);
		m_buyEstate->setEnabled(true);
		m_endTurn->setEnabled(true);
		m_jailCard->setEnabled(true);
		m_jailPay->setEnabled(true);
		m_jailRoll->setEnabled(true);
	}
	else
	{
		m_roll->setEnabled(false);
		m_buyEstate->setEnabled(false);
		m_endTurn->setEnabled(false);
		m_jailCard->setEnabled(false);
		m_jailRoll->setEnabled(false);
		m_jailPay->setEnabled(false);
	}
	m_board->raiseToken(playerId);

	for (QMap<int, Player *>::Iterator i=playerMap.begin() ; i != playerMap.end() ; ++i)
	{
		if ((player = *i))
			player->setHasTurn(player->playerId()==playerId);
	}
}

void Atlantik::slotPlayerInit(int playerId)
{
	Player *player;
	if (!(player = playerMap[playerId]))
	{
		player = new Player(playerId);
		playerMap[playerId] = player;

		m_board->addToken(player);

		PortfolioView *portfolioView = new PortfolioView(player, m_portfolioWidget);
		portfolioMap[playerId] = portfolioView;

		Estate *estate;
		for (QMap<int, Estate *>::Iterator i=estateMap.begin() ; i != estateMap.end() ; ++i)
			{
				kdDebug() << "estate entry found" << endl;
				if ((estate = *i))
				{
					kdDebug() << "portfolioView->addEstateView" << endl;
					portfolioView->addEstateView(estate);
				}
			}

		connect(player, SIGNAL(changed()), portfolioView, SLOT(playerChanged()));
		connect(portfolioView, SIGNAL(newTrade(Player *)), m_gameNetwork, SLOT(newTrade(Player *)));

		m_portfolioLayout->addWidget(portfolioView);
		portfolioView->show();
	}
}

void Atlantik::slotEstateInit(int estateId)
{
	kdDebug() << "Atlantik::slotEstateInit(" << estateId << ")" << endl;
	Estate *estate;
	if (!(estate = estateMap[estateId]))
	{
		estate = new Estate(estateId);

		connect(estate, SIGNAL(estateToggleMortgage(int)), m_gameNetwork, SLOT(estateToggleMortgage(int)));
		connect(estate, SIGNAL(estateHouseBuy(int)), m_gameNetwork, SLOT(estateHouseBuy(int)));
		connect(estate, SIGNAL(estateHouseSell(int)), m_gameNetwork, SLOT(estateHouseSell(int)));

		estateMap[estateId] = estate;

		m_board->addEstateView(estate);

		PortfolioView *portfolioView;
		for (QMap<int, PortfolioView *>::Iterator i=portfolioMap.begin() ; i != portfolioMap.end() ; ++i)
			{
				kdDebug() << "portfolio entry found" << endl;
				if ((portfolioView = *i))
				{
					kdDebug() << "portfolioView->addEstateView" << endl;
					portfolioView->addEstateView(estate);
				}
			}
	}
}

void Atlantik::slotTradeInit(int tradeId)
{
	Trade *trade;
	if (!(trade = tradeMap[tradeId]))
	{
		trade = new Trade(m_gameNetwork, tradeId);

		TradeDisplay *tradeDisplay = new TradeDisplay(trade);
		tradeDisplay->show();

		tradeMap[tradeId] = trade;

//		m_board->addTradeView(trade);
	}
}

void Atlantik::slotTradeUpdatePlayerAdd(int tradeId, int playerId)
{
	Trade *trade;
	Player *player;
	if ((trade = tradeMap[tradeId]) && (player = playerMap[playerId]))
		trade->addPlayer(player);
}

void Atlantik::slotTradeUpdateEstate(int tradeId, int estateId, int playerId)
{
	Trade *trade = tradeMap[tradeId];
	Player *player = playerMap[playerId];
	Estate *estate = estateMap[estateId];

	if (trade && estate)
		trade->updateEstate(estate, player);
}

void Atlantik::slotTradeUpdateMoney(int tradeId, int playerFromId, int playerToId, unsigned int money)
{
	Trade *trade = tradeMap[tradeId];
	Player *pFrom = playerMap[playerFromId];
	Player *pTo = playerMap[playerToId];

	if (trade && pFrom && pTo)
		trade->updateMoney(pFrom, pTo, money);
}

void Atlantik::serverMsgsAppend(QString msg)
{
	// Use append, not setText(old+new) because that one doesn't wrap
	m_serverMsgs->append(msg+"<br>");
	m_serverMsgs->ensureVisible(0, m_serverMsgs->contentsHeight());
}
