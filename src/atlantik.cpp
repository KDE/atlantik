#include <qlineedit.h>
#include <qscrollbar.h>

#include <kstdgameaction.h>

#include <kstdaction.h>
#include <ktoolbar.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
 
#include "atlantik.moc"
#include "board.h"

#include "config.h"

extern AtlantikConfig atlantikConfig;

Atlantik::Atlantik () :
  KMainWindow ()
{
	// Read application configuration
	readConfig();

	// Toolbar: Game
	KStdGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
	KStdGameAction::quit(kapp, SLOT(closeAllWindows()), actionCollection(), "game_quit");

	// Toolbar: Move
	m_roll = KStdGameAction::roll(this, SLOT(slotRoll()), actionCollection()); // No Ctrl-R at the moment
	m_roll->setEnabled(false);
	m_buyEstate = new KAction("&Buy", "atlantik_buy_estate", CTRL+Key_B, this, SLOT(slotBuy()), actionCollection(), "buy_estate");
	m_buyEstate->setEnabled(false);
	m_endTurn = KStdGameAction::endTurn(this, SLOT(slotEndTurn()), actionCollection());
	m_endTurn->setEnabled(false);

	// Toolbar: Settings
	KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());

	// Initialize pointers to 0L
	m_configDialog = 0;
	m_newgameWizard = 0;

	// Mix code and XML into GUI
	createGUI();

	// Network layer
	gameNetwork = new GameNetwork(this, "gameNetwork");

	connect(gameNetwork, SIGNAL(msgError(QString)), this, SLOT(slotMsgError(QString)));
	connect(gameNetwork, SIGNAL(msgInfo(QString)), this, SLOT(slotMsgInfo(QString)));
	connect(gameNetwork, SIGNAL(msgChat(QString, QString)), this, SLOT(slotMsgChat(QString, QString)));
	connect(gameNetwork, SIGNAL(msgStartGame(QString)), this, SLOT(slotMsgStartGame(QString)));
	connect(gameNetwork, SIGNAL(msgPlayerUpdateName(int, QString)), this, SLOT(slotMsgPlayerUpdateName(int, QString)));
	connect(gameNetwork, SIGNAL(msgPlayerUpdateMoney(int, QString)), this, SLOT(slotMsgPlayerUpdateMoney(int, QString)));
	connect(gameNetwork, SIGNAL(msgEstateUpdateOwner(int, int)), this, SLOT(slotMsgEstateUpdateOwner(int, int)));
	connect(gameNetwork, SIGNAL(msgEstateUpdateName(int, QString)), this, SLOT(slotMsgEstateUpdateName(int, QString)));
	connect(gameNetwork, SIGNAL(msgEstateUpdateHouses(int, int)), this, SLOT(slotMsgEstateUpdateHouses(int, int)));
	connect(gameNetwork, SIGNAL(msgEstateUpdateMortgaged(int, bool)), this, SLOT(slotMsgEstateUpdateMortgaged(int, bool)));
	connect(gameNetwork, SIGNAL(msgEstateUpdateCanBeMortgaged(int, bool)), this, SLOT(slotMsgEstateUpdateCanBeMortgaged(int, bool)));
	connect(gameNetwork, SIGNAL(msgEstateUpdateCanBeUnmortgaged(int, bool)), this, SLOT(slotMsgEstateUpdateCanBeUnmortgaged(int, bool)));
	connect(gameNetwork, SIGNAL(msgEstateUpdateCanBeOwned(int, bool)), this, SLOT(slotMsgEstateUpdateCanBeOwned(int, bool)));
	connect(gameNetwork, SIGNAL(setPlayerId(int)), this, SLOT(slotSetPlayerId(int)));
	connect(gameNetwork, SIGNAL(setTurn(int)), this, SLOT(slotSetTurn(int)));

	// Management of data objects (players, games, estates)
	playerList.setAutoDelete(true);
	estateList.setAutoDelete(true);

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
	connect(gameNetwork, SIGNAL(playerInit(int)), this, SLOT(slotPlayerInit(int)));
	connect(gameNetwork, SIGNAL(estateInit(int)), this, SLOT(slotEstateInit(int)));

	// Nice label
	m_portfolioLabel = new QLabel(i18n("Players"), m_portfolioWidget, "pfLabel");
	m_portfolioLayout->addWidget(m_portfolioLabel);
	m_portfolioLabel->show();

	// TextView for chat and status messages from server.
	m_serverMsgs = new QTextView(m_mainWidget, "serverMsgs");
	m_serverMsgs->setHScrollBarMode(QScrollView::AlwaysOff);
	m_serverMsgs->setFixedWidth(225);
	m_mainLayout->addWidget(m_serverMsgs, 1, 0);

	// LineEdit to enter commands and chat messages.
	m_input = new QLineEdit(m_mainWidget, "input");
	connect(m_input, SIGNAL(returnPressed()), this, SLOT(slotSendMsg()));
	m_mainLayout->addWidget(m_input, 2, 0);

	// The actual gameboard.
	m_board = new AtlantikBoard(m_mainWidget, "board");
	m_mainLayout->addMultiCellWidget(m_board, 0, 2, 1, 1);

	// Set stretching where we want it.
	m_mainLayout->setRowStretch(1, 1); // make m_board+m_serverMsgs stretch vertically, not the rest
	m_mainLayout->setColStretch(1, 1); // make m_board stretch horizontally, not the rest
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

void Atlantik::slotNewGame()
{
	m_newgameWizard = new NewGameWizard(this, "newgame", 1);
	int result = m_newgameWizard->exec();
	delete m_newgameWizard;
	m_newgameWizard = 0;
	if (result)
		gameNetwork->cmdGameStart();
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
		gameNetwork->cmdName(optStr);
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
		m_board->redrawEstates();
}

void Atlantik::slotRoll()
{
	gameNetwork->cmdRoll();
}

void Atlantik::slotBuy()
{
	gameNetwork->cmdBuyEstate();
}

void Atlantik::slotEndTurn()
{
	gameNetwork->cmdEndTurn();
}

void Atlantik::slotSendMsg()
{
	gameNetwork->cmdChat(m_input->text());
	m_input->setText("");
}

void Atlantik::slotMsgError(QString msg)
{
	serverMsgsAppend("ERR: " + msg);
}

void Atlantik::slotMsgInfo(QString msg)
{
	serverMsgsAppend(msg);
}

void Atlantik::slotMsgChat(QString player, QString msg)
{
	serverMsgsAppend("<b>" + player + ":</b> " + msg);
}

void Atlantik::slotMsgStartGame(QString msg)
{
	if (m_newgameWizard!=0)
		m_newgameWizard->hide();
		
	serverMsgsAppend("START: " + msg);
}

void Atlantik::slotMsgPlayerUpdateName(int playerid, QString name)
{
	Player *player = playerMap[playerid];
	if (player)
		player->setName(name);

// TODO: port to player class
//		QString label;
//		label.setNum(playerid);
//		label.append(". " + name);
//		m_portfolioArray[playerid]->setName(label);
}

void Atlantik::slotMsgPlayerUpdateMoney(int playerid, QString money)
{
	Player *player = playerMap[playerid];
	if (player)
		player->setMoney("$ " + money);
}

void Atlantik::slotMsgEstateUpdateOwner(int estateId, int playerId)
{
	Estate *estate = estateMap[estateId];
	Player *player = playerMap[playerId];

	if (estate)
		estate->setOwner(player);
		
#warning port slotMsgEstateUpdateOwner
/*
	if (estateId < 40 && playerId < MAXPLAYERS)
	{
		// Update all portfolio estates.
		for(int i=0;i<MAXPLAYERS;i++)
			if (m_portfolioArray[i]!=0)
				m_portfolioArray[i]->setOwned(estateId, (playerId == i ? true : false));
	}
*/
}

void Atlantik::slotMsgEstateUpdateName(int estateId, QString name)
{
	if (Estate *estate = estateMap[estateId])
		estate->setName(name);
}

void Atlantik::slotMsgEstateUpdateHouses(int estateid, int houses)
{
//	if (Estate *estate = estateMap[estateId])
//		estate->setHouses(houses);
}

void Atlantik::slotMsgEstateUpdateMortgaged(int estateid, bool mortgaged)
{
//	if (Estate *estate = estateMap[estateId])
//		estate->setMortgaged(mortgaged);
}

void Atlantik::slotMsgEstateUpdateCanBeMortgaged(int estateid, bool mortgaged)
{
//	if (Estate *estate = estateMap[estateId])
//		estate->setCanBeMortgaged(mortgaged);
}

void Atlantik::slotMsgEstateUpdateCanBeUnmortgaged(int estateid, bool mortgaged)
{
//	if (Estate *estate = estateMap[estateId])
//		estate->setCanBeUnmortgaged(mortgaged);
}

void Atlantik::slotMsgEstateUpdateCanBeOwned(int estateId, bool canBeOwned)
{
	if (Estate *estate = estateMap[estateId])
		estate->setCanBeOwned(canBeOwned);
}

void Atlantik::slotSetPlayerId(int playerId)
{
	Player *player = playerMap[playerId];
	if (player)
		player->setIsSelf(true);
}

void Atlantik::slotSetTurn(int playerid)
{
#warning port slotSetTurn
/*
	if (playerid == m_myPlayerId)
	{
		m_roll->setEnabled(true);
		m_buyEstate->setEnabled(true);
		m_endTurn->setEnabled(true);
	}
	else
	{
		m_roll->setEnabled(false);
		m_buyEstate->setEnabled(false);
		m_endTurn->setEnabled(false);
	}

	m_board->raiseToken(playerid);


	for(int i=0 ; i<MAXPLAYERS ; i++)
	{
		if (m_portfolioArray[i]!=0)
		{
			m_portfolioArray[i]->setHasTurn(i==playerid ? true : false);
		}
	}
*/
}

void Atlantik::slotPlayerInit(int playerid)
{
	Player *player;
	if (!(player = playerMap[playerid]))
	{
		player = new Player(playerid);
		playerList.append(player);
		playerMap[playerid] = player;

		PortfolioView *fpv = new PortfolioView(m_portfolioWidget);
		m_portfolioLayout->addWidget(fpv);
		fpv->show();
		player->setView(fpv);
	}
}

void Atlantik::slotEstateInit(int estateId)
{
	Estate *estate;
	if (!(estate = estateMap[estateId]))
	{
		estate = new Estate(estateId);
		estateList.append(estate);
		estateMap[estateId] = estate;

		m_board->addEstateView(estate);
	}
}

void Atlantik::serverMsgsAppend(QString msg)
{
	// Use append, not setText(old+new) because that one doesn't wrap
	m_serverMsgs->append(msg);
	m_serverMsgs->ensureVisible(0, m_serverMsgs->contentsHeight());
}
