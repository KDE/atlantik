#include <qlineedit.h>
#include <qscrollbar.h>

#include <kstdgameaction.h>

#include <kstdaction.h>
#include <ktoolbar.h>
#include <kapplication.h>
#include <kconfig.h>

#include "atlantik.moc"
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
	connect(gameNetwork, SIGNAL(setPlayerId(int)), this, SLOT(slotSetPlayerId(int)));
	connect(gameNetwork, SIGNAL(setTurn(int)), this, SLOT(slotSetTurn(int)));

	// Main widget, containing all others
 	m_mainWidget = new QWidget(this, "main");
	m_mainWidget->show();
	m_mainLayout = new QGridLayout(m_mainWidget, 9, 2);
	setCentralWidget(m_mainWidget);

	// TextView for chat and status messages from server.
	m_serverMsgs = new QTextView(m_mainWidget, "serverMsgs");
	m_serverMsgs->setHScrollBarMode(QScrollView::AlwaysOff);
	m_serverMsgs->setFixedWidth(225);
	m_mainLayout->addWidget(m_serverMsgs, 6, 0);

	// LineEdit to enter commands and chat messages.
	m_input = new QLineEdit(m_mainWidget, "input");
	connect(m_input, SIGNAL(returnPressed()), this, SLOT(slotSendMsg()));
	m_mainLayout->addWidget(m_input, 7, 0);

	// The actual gameboard.
	m_board = new AtlantikBoard(m_mainWidget, "board");
	m_mainLayout->addMultiCellWidget(m_board, 0, 7, 1, 1);

	m_mainLayout->setRowStretch(6, 1); // make m_board+m_serverMsgs stretch vertically, not the rest
	m_mainLayout->setColStretch(1, 1); // make m_board stretch horizontally, not the rest

	// Regarding our game connection. TODO: Possibly move to gameNetwork,
	// since it's related to our current connection.
	m_myPlayerId = -1;

	// Initialize portfolioviews. TODO: Make a nice vertical layout where we can add them dynamically.
	for(int i=0;i<MAXPLAYERS;i++)
	{
		m_portfolioArray[i] = new PortfolioView(m_mainWidget);
		m_mainLayout->addWidget(m_portfolioArray[i], i, 0);
		m_portfolioArray[i]->hide();
	}
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
	if (playerid >=0 && playerid < MAXPLAYERS && m_portfolioArray[playerid]!=0)
	{
		if (m_portfolioArray[playerid]->isHidden())
			m_portfolioArray[playerid]->show();

		QString label;
		label.setNum(playerid);
		label.append(". " + name);
		m_portfolioArray[playerid]->setName(label);
	}
}

void Atlantik::slotMsgPlayerUpdateMoney(int playerid, QString money)
{
	if (playerid >=0 && playerid < MAXPLAYERS && m_portfolioArray[playerid]!=0)
	{
		if (m_portfolioArray[playerid]->isHidden())
			m_portfolioArray[playerid]->show();

		m_portfolioArray[playerid]->setCash("$ " + money);
	}
}

void Atlantik::slotMsgEstateUpdateOwner(int estateId, int playerId)
{
	if (estateId < 40 && playerId < MAXPLAYERS)
	{
		// Update all portfolio estates.
		for(int i=0;i<MAXPLAYERS;i++)
			if (m_portfolioArray[i]!=0)
				m_portfolioArray[i]->setOwned(estateId, (playerId == i ? true : false));

		// Update gameboard.
		m_board->setOwned(estateId, (playerId == -1 ? false : true), (playerId == m_myPlayerId ? true : false));
	}
}

void Atlantik::slotSetPlayerId(int playerId)
{
	m_myPlayerId = playerId;
}

void Atlantik::slotSetTurn(int playerid)
{

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
}

void Atlantik::serverMsgsAppend(QString msg)
{
	// Use append, not setText (old+new) because that one doesn't wrap
	m_serverMsgs->append(msg);
	m_serverMsgs->ensureVisible(0, m_serverMsgs->contentsHeight());
#warning fixed in qt 3.0
	m_serverMsgs->viewport()->update();
}
