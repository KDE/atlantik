#include <qlineedit.h>
#include <qscrollbar.h>

#include <kstdaction.h>
#include <ktoolbar.h>
#include <kapp.h>

#include "kmonop.moc"
#include "config.h"

KMonop::KMonop (const char *name) :
  KTMainWindow (name)
{
	// Game actions
	KStdAction::openNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
	KStdAction::quit(kapp, SLOT(closeAllWindows()), actionCollection(), "game_quit");

	// Move actions
	roll_die = new KAction("&Roll", "kmonop_roll_die", CTRL+Key_R, this, SLOT(slotRoll()), actionCollection(), "roll_die");
	roll_die->setEnabled(false);
	buy_estate = new KAction("&Buy estate", "kmonop_buy_estate", CTRL+Key_B, this, SLOT(slotBuy()), actionCollection(), "buy_estate");
	buy_estate->setEnabled(false);

	// Settings actions
	config_kmonop = new KAction("&Configure KMonop", "configure", 0, this, SLOT(slotConfigure()), actionCollection(), "config_kmonop");
//	config_kmonop->setEnabled(false); // since its not done yet

	createGUI();
	toolBar(0)->setBarPos(KToolBar::Left);

	wizard = 0;
	netw = new GameNetwork(this, "network");

	connect(netw, SIGNAL(msgError(QString)), this, SLOT(slotMsgError(QString)));
	connect(netw, SIGNAL(msgInfo(QString)), this, SLOT(slotMsgInfo(QString)));
	connect(netw, SIGNAL(msgStartGame(QString)), this, SLOT(slotMsgStartGame(QString)));
	connect(netw, SIGNAL(msgPlayerUpdate(QDomNode)), this, SLOT(slotMsgPlayerUpdate(QDomNode)));
	connect(netw, SIGNAL(msgEstateUpdate(int, int)), this, SLOT(slotMsgEstateUpdate(int, int)));
	connect(netw, SIGNAL(setPlayerId(int)), this, SLOT(slotSetPlayerId(int)));
	connect(netw, SIGNAL(setTurn(int)), this, SLOT(slotSetTurn(int)));

 	main = new QWidget(this, "main");
	main->show();

	layout = new QGridLayout(main, 8, 2);

	output = new QTextView(main, "output");
	output->setHScrollBarMode(QScrollView::AlwaysOff);
	output->setFixedWidth(225);

	input = new QLineEdit(main, "input");
	connect(input, SIGNAL(returnPressed()), this, SLOT(slotSendMsg()));

	board = new KMonopBoard(netw, main, "board");

	layout->addWidget(output, 6, 0);
	layout->addWidget(input, 7, 0);
	layout->addMultiCellWidget(board, 0, 7, 1, 1);
	layout->setRowStretch(6, 1); // make board+output stretch, not the rest
	layout->setColStretch(1, 1); // make board stretch, not the rest

	myPlayerId = -1;

	for(int i=0;i<MAXPLAYERS;i++)
		port[i]=0;

	setView(main);
}

void KMonop::slotNewGame()
{
	int result;

	wizard = new NewGameWizard(netw, this, "newgame", 1);
	result = wizard->exec();
	delete wizard;
	wizard = 0;
	if (result)
		netw->writeData(".gs");
}

void KMonop::slotConfigure()
{
	if (configDialog == 0)
	{
		configDialog = new ConfigDialog(this);
	}
	configDialog->show();
//	configDialog->raise();
}

void KMonop::slotRoll()
{
	netw->writeData(".r");
}

void KMonop::slotBuy()
{
	netw->writeData(".b");
}

void KMonop::slotSendMsg()
{
	QString str(input->text());
	netw->writeData(str.latin1());
	input->setText("");
}

void KMonop::slotMsgError(QString msg)
{
	outputAppend("ERR: " + msg);
}

void KMonop::slotMsgInfo(QString msg)
{
	outputAppend(msg);
}

void KMonop::slotMsgStartGame(QString msg)
{
	if (wizard!=0)
		wizard->hide();
		
	outputAppend("START: " + msg);
}

void KMonop::slotMsgPlayerUpdate(QDomNode playerupdate)
{
	QDomAttr a_id, a_name, a_money, a_location;
	QDomElement e;
	int id=0;

	e = playerupdate.toElement();
	if(!e.isNull())
	{
		a_id = e.attributeNode(QString("id"));
		a_name = e.attributeNode(QString("name"));
		a_money = e.attributeNode(QString("money"));
		a_location = e.attributeNode(QString("location"));

		// Maximum of six players, right? Check with monopd, possibly fetch
		// number first and make portfolio overviews part of QHLayout to
		// allow any theoretically number.
		id = a_id.value().toInt() - 1;
		if (id < MAXPLAYERS)
		{
			// Only create portfolio once.
			if(port[id]==0)
			{
				port[id] = new PortfolioView(main);
				port[id]->show();
				layout->addWidget(port[id], id, 0);
			}
			port[id]->setName(a_id.value() + ". " + a_name.value());
			port[id]->setCash("$ " + a_money.value());
		}
	}
}

void KMonop::slotMsgEstateUpdate(int id, int owner)
{
	// Decrease owner because array is 0-indexed
	owner--;

	if (id < 40 && owner < MAXPLAYERS)
	{
		if (port[owner]!=0)
			port[owner]->setOwned(id, true);
		board->setOwned(id, true);
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
	}
	else
	{
		roll_die->setEnabled(false);
		buy_estate->setEnabled(false);
	}

	// Decrease player because array is 0-indexed
	player--;

	board->raiseToken(player);

	for(int i=0 ; i<MAXPLAYERS ; i++)
	{
		if (port[i]!=0)
		{
			port[i]->setHasTurn(i==player ? true : false);
		}
	}
}

void KMonop::outputAppend(QString msg)
{
	output->append(msg);
	output->ensureVisible(0, output->contentsHeight());
#warning fixed in qt 3.0
	output->viewport()->update();
}
