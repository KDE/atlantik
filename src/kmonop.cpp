#include <qlineedit.h>
#include <qscrollbar.h>

// Use hardcoded path while in kdenonbeta
// #include <kstdgameaction.h>
#include "../../libkdegames/kstdgameaction.h"

#include <kstdaction.h>
#include <ktoolbar.h>
#include <kapp.h>
#include <kconfig.h>

#include "kmonop.moc"
#include "config.h"

extern KMonopConfig kmonopConfig;

KMonop::KMonop (const char *name) :
  KTMainWindow (name)
{
	readConfig();

	// Game actions
	KStdGameAction::gameNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
	KStdGameAction::quit(kapp, SLOT(closeAllWindows()), actionCollection(), "game_quit");

	// Toolbar actions
	m_roll = KStdGameAction::roll(this, SLOT(slotRoll()), actionCollection()); // No Ctrl-R at the moment
	m_roll->setEnabled(false);
	m_buyEstate = new KAction("&Buy", "kmonop_buy_estate", CTRL+Key_B, this, SLOT(slotBuy()), actionCollection(), "buy_estate");
	m_buyEstate->setEnabled(false);
	m_endTurn = KStdGameAction::endTurn(this, SLOT(slotEndTurn()), actionCollection());
	m_endTurn->setEnabled(false);

	// Settings actions
	KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());

	// Initialize pointers to 0L
	m_configDialog = 0;
	m_newgameWizard = 0;

	createGUI();

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

 	m_mainWidget = new QWidget(this, "main");
	m_mainWidget->show();

	m_mainLayout = new QGridLayout(m_mainWidget, 9, 2);

	m_serverMsgs = new QTextView(m_mainWidget, "serverMsgs");
	m_serverMsgs->setHScrollBarMode(QScrollView::AlwaysOff);
	m_serverMsgs->setFixedWidth(225);

	m_input = new QLineEdit(m_mainWidget, "input");
	connect(m_input, SIGNAL(returnPressed()), this, SLOT(slotSendMsg()));

	m_board = new KMonopBoard(m_mainWidget, "board");

	m_mainLayout->addWidget(m_serverMsgs, 6, 0);
	m_mainLayout->addWidget(m_input, 7, 0);
	m_mainLayout->addMultiCellWidget(m_board, 0, 7, 1, 1);
	m_mainLayout->setRowStretch(6, 1); // make m_board+m_serverMsgs stretch, not the rest
	m_mainLayout->setColStretch(1, 1); // make m_board stretch, not the rest

	m_myPlayerId = -1;

	for(int i=0;i<MAXPLAYERS;i++)
	{
		m_portfolioArray[i] = new PortfolioView(m_mainWidget);
		m_mainLayout->addWidget(m_portfolioArray[i], i, 0);
		m_portfolioArray[i]->hide();
	}

	setView(m_mainWidget);
}

void KMonop::readConfig()
{
	KConfig *config=kapp->config();

	config->setGroup("Personalization");
	kmonopConfig.playerName = config->readEntry("PlayerName", "KMonop");

	config->setGroup("Board");
	kmonopConfig.indicateUnowned = config->readBoolEntry("IndicateUnowned", true);
	kmonopConfig.highliteUnowned = config->readBoolEntry("HighliteUnowned", false);
	kmonopConfig.grayOutMortgaged = config->readBoolEntry("GrayOutMortgaged", true);
	kmonopConfig.animateToken = config->readBoolEntry("AnimateToken", false);
}

void KMonop::slotNewGame()
{
	int result;

	m_newgameWizard = new NewGameWizard(this, "newgame", 1);
	result = m_newgameWizard->exec();
	delete m_newgameWizard;
	m_newgameWizard = 0;
	if (result)
		gameNetwork->writeData(".gs");
}

void KMonop::slotConfigure()
{
	if (m_configDialog == 0)
		m_configDialog = new ConfigDialog(this);
	m_configDialog->show();
	
	connect(m_configDialog, SIGNAL(okClicked()), this, SLOT(slotUpdateConfig()));
}

void KMonop::slotUpdateConfig()
{
	KConfig *config=kapp->config();
	bool optBool, redrawEstates = false;
	QString optStr;

	optStr = m_configDialog->playerName();
	if (kmonopConfig.playerName != optStr)
	{
		kmonopConfig.playerName = optStr;
		gameNetwork->writeData(".n" + optStr);
	}

	optBool = m_configDialog->indicateUnowned();
	if (kmonopConfig.indicateUnowned != optBool)
	{
		kmonopConfig.indicateUnowned = optBool;
		m_board->indicateUnownedChanged();
	}

	optBool = m_configDialog->highliteUnowned();
	if (kmonopConfig.highliteUnowned != optBool)
	{
		kmonopConfig.highliteUnowned = optBool;
		redrawEstates = true;
	}

	optBool = m_configDialog->grayOutMortgaged();
	if (kmonopConfig.grayOutMortgaged != optBool)
	{
		kmonopConfig.grayOutMortgaged = optBool;
		redrawEstates = true;
	}

	optBool = m_configDialog->animateToken();
	if (kmonopConfig.animateToken != optBool)
	{
		kmonopConfig.animateToken = optBool;
	}

	config->setGroup("Personalization");
	config->writeEntry("PlayerName", kmonopConfig.playerName);

	config->setGroup("Board");
	config->writeEntry("IndicateUnowned", kmonopConfig.indicateUnowned);
	config->writeEntry("HighliteUnowned", kmonopConfig.highliteUnowned);
	config->writeEntry("GrayOutMortgaged", kmonopConfig.grayOutMortgaged);
	config->writeEntry("AnimateToken", kmonopConfig.animateToken);

	config->sync();

	if (redrawEstates)
		m_board->redrawEstates();
}

void KMonop::slotRoll()
{
	gameNetwork->writeData(".r");
}

void KMonop::slotBuy()
{
	gameNetwork->writeData(".be");
}

void KMonop::slotEndTurn()
{
	gameNetwork->writeData(".e");
}

void KMonop::slotSendMsg()
{
	QString str(m_input->text());
	gameNetwork->writeData(str.latin1());
	m_input->setText("");
}

void KMonop::slotMsgError(QString msg)
{
	serverMsgsAppend("ERR: " + msg);
}

void KMonop::slotMsgInfo(QString msg)
{
	serverMsgsAppend(msg);
}

void KMonop::slotMsgChat(QString player, QString msg)
{
	serverMsgsAppend("<b>" + player + ":</b> " + msg);
}

void KMonop::slotMsgStartGame(QString msg)
{
	if (m_newgameWizard!=0)
		m_newgameWizard->hide();
		
	serverMsgsAppend("START: " + msg);
}

void KMonop::slotMsgPlayerUpdateName(int playerid, QString name)
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

void KMonop::slotMsgPlayerUpdateMoney(int playerid, QString money)
{
	if (playerid >=0 && playerid < MAXPLAYERS && m_portfolioArray[playerid]!=0)
	{
		if (m_portfolioArray[playerid]->isHidden())
			m_portfolioArray[playerid]->show();

		m_portfolioArray[playerid]->setCash("$ " + money);
	}
}

void KMonop::slotMsgEstateUpdateOwner(int estateid, int playerid)
{
	if (estateid < 40 && playerid < MAXPLAYERS)
	{
		if (playerid == -1)
		{
			for(int i=0;i<MAXPLAYERS;i++)
				if (m_portfolioArray[i]!=0)
					m_portfolioArray[i]->setOwned(estateid, false);
			m_board->setOwned(estateid, false);
		}
		else
		{
			if (m_portfolioArray[playerid]!=0)
				m_portfolioArray[playerid]->setOwned(estateid, true);
			m_board->setOwned(estateid, true);
		}
	}
}

void KMonop::slotSetPlayerId(int id)
{
	m_myPlayerId = id;
}

void KMonop::slotSetTurn(int playerid)
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

void KMonop::serverMsgsAppend(QString msg)
{
	m_serverMsgs->append(msg);
	m_serverMsgs->ensureVisible(0, m_serverMsgs->contentsHeight());
#warning fixed in qt 3.0
	m_serverMsgs->viewport()->update();
}
