#include <qlineedit.h>
#include <qscrollbar.h>

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
	KStdAction::openNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
	KStdAction::quit(kapp, SLOT(closeAllWindows()), actionCollection(), "game_quit");

	// Toolbar actions
	roll_die = new KAction("&Roll", "kmonop_roll_die", CTRL+Key_R, this, SLOT(slotRoll()), actionCollection(), "roll_die");
	roll_die->setEnabled(false);
	buy_estate = new KAction("&Buy", "kmonop_buy_estate", CTRL+Key_B, this, SLOT(slotBuy()), actionCollection(), "buy_estate");
	buy_estate->setEnabled(false);
	end_turn = new KAction("&End Turn", "stop", CTRL+Key_E, this, SLOT(slotEndTurn()), actionCollection(), "end_turn");
	end_turn->setEnabled(false);

	// Settings actions
	config_kmonop = new KAction("&Configure KMonop", "configure", 0, this, SLOT(slotConfigure()), actionCollection(), "config_kmonop");

	// Initialize pointers to 0L
	configDialog = 0;
	wizard = 0;

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

 	main = new QWidget(this, "main");
	main->show();

	layout = new QGridLayout(main, 9, 2);

	serverMsgs = new QTextView(main, "serverMsgs");
	serverMsgs->setHScrollBarMode(QScrollView::AlwaysOff);
	serverMsgs->setFixedWidth(225);

	input = new QLineEdit(main, "input");
	connect(input, SIGNAL(returnPressed()), this, SLOT(slotSendMsg()));

	board = new KMonopBoard(main, "board");

	layout->addWidget(serverMsgs, 6, 0);
	layout->addWidget(input, 7, 0);
	layout->addMultiCellWidget(board, 0, 7, 1, 1);
	layout->setRowStretch(6, 1); // make board+serverMsgs stretch, not the rest
	layout->setColStretch(1, 1); // make board stretch, not the rest

	myPlayerId = -1;

	for(int i=0;i<MAXPLAYERS;i++)
	{
		port[i] = new PortfolioView(main);
		layout->addWidget(port[i], i, 0);
		port[i]->hide();
	}

	setView(main);
}

void KMonop::readConfig()
{
	KConfig *config=kapp->config();

	config->setGroup("Personalization");
	kmonopConfig.playerName = config->readEntry("PlayerName", "KMonop");

	config->setGroup("Board");
	kmonopConfig.indicateUnowned = config->readBoolEntry("IndicateUnowned", true);
	kmonopConfig.highliteUnowned = config->readBoolEntry("HighliteUnowned", true);
	kmonopConfig.grayOutMortgaged = config->readBoolEntry("GrayOutMortgaged", true);
	kmonopConfig.animateToken = config->readBoolEntry("AnimateToken", false);
}

void KMonop::slotNewGame()
{
	int result;

	wizard = new NewGameWizard(this, "newgame", 1);
	result = wizard->exec();
	delete wizard;
	wizard = 0;
	if (result)
		gameNetwork->writeData(".gs");
}

void KMonop::slotConfigure()
{
	if (configDialog == 0)
	{
		configDialog = new ConfigDialog(this);
	}
	configDialog->show();
	
	connect(configDialog, SIGNAL(okClicked()), this, SLOT(slotUpdateConfig()));
}

void KMonop::slotUpdateConfig()
{
	KConfig *config=kapp->config();
	bool optBool, redrawEstates = false;
	QString optStr;

	optStr = configDialog->playerName();
	if (kmonopConfig.playerName != optStr)
	{
		kmonopConfig.playerName = optStr;
		gameNetwork->writeData(".n" + optStr);
	}

	optBool = configDialog->indicateUnowned();
	if (kmonopConfig.indicateUnowned != optBool)
	{
		kmonopConfig.indicateUnowned = optBool;
		board->indicateUnownedChanged();
	}

	optBool = configDialog->highliteUnowned();
	if (kmonopConfig.highliteUnowned != optBool)
	{
		kmonopConfig.highliteUnowned = optBool;
		redrawEstates = true;
	}

	optBool = configDialog->grayOutMortgaged();
	if (kmonopConfig.grayOutMortgaged != optBool)
	{
		kmonopConfig.grayOutMortgaged = optBool;
		redrawEstates = true;
	}

	optBool = configDialog->animateToken();
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
		board->redrawEstates();
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
	QString str(input->text());
	gameNetwork->writeData(str.latin1());
	input->setText("");
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
	if (wizard!=0)
		wizard->hide();
		
	serverMsgsAppend("START: " + msg);
}

void KMonop::slotMsgPlayerUpdateName(int playerid, QString name)
{
	if (playerid >=0 && playerid < MAXPLAYERS && port[playerid]!=0)
	{
		if (port[playerid]->isHidden())
			port[playerid]->show();

		QString label;
		label.setNum(playerid);
		label.append(". " + name);
		port[playerid]->setName(label);
	}
}

void KMonop::slotMsgPlayerUpdateMoney(int playerid, QString money)
{
	if (playerid >=0 && playerid < MAXPLAYERS && port[playerid]!=0)
	{
		if (port[playerid]->isHidden())
			port[playerid]->show();

		port[playerid]->setCash("$ " + money);
	}
}

void KMonop::slotMsgEstateUpdateOwner(int estateid, int playerid)
{
	if (estateid < 40 && playerid < MAXPLAYERS)
	{
		if (playerid == -1)
		{
			for(int i=0;i<MAXPLAYERS;i++)
				if (port[i]!=0)
					port[i]->setOwned(estateid, false);
			board->setOwned(estateid, false);
		}
		else
		{
			if (port[playerid]!=0)
				port[playerid]->setOwned(estateid, true);
			board->setOwned(estateid, true);
		}
	}
}

void KMonop::slotSetPlayerId(int id)
{
	myPlayerId = id;
}

void KMonop::slotSetTurn(int player)
{

	if (player == myPlayerId)
	{
		roll_die->setEnabled(true);
		buy_estate->setEnabled(true);
		end_turn->setEnabled(true);
	}
	else
	{
		roll_die->setEnabled(false);
		buy_estate->setEnabled(false);
		end_turn->setEnabled(false);
	}

	board->raiseToken(player);

	for(int i=0 ; i<MAXPLAYERS ; i++)
	{
		if (port[i]!=0)
		{
			port[i]->setHasTurn(i==player ? true : false);
		}
	}
}

void KMonop::serverMsgsAppend(QString msg)
{
	serverMsgs->append(msg);
	serverMsgs->ensureVisible(0, serverMsgs->contentsHeight());
#warning fixed in qt 3.0
	serverMsgs->viewport()->update();
}
