#include <qlineedit.h>
#include <iostream.h>

#include <kstdaction.h>
#include <ktoolbar.h>
#include <kapp.h>

#include "kmonop.moc"
#include "config.h"
#include "board.h"

KMonop::KMonop (const char *name) :
  KTMainWindow (name)
{
	KStdAction::openNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
	KStdAction::quit(kapp, SLOT(closeAllWindows()), actionCollection(), "game_quit");

	roll_die = new KAction("&Roll", "kmonop_roll_die", CTRL+Key_R, this, SLOT(slotRoll()), actionCollection(), "roll_die");
//	roll_die->setEnabled(false);
	buy_estate = new KAction("&Buy estate", "kmonop_buy_estate", CTRL+Key_B, this, SLOT(slotBuy()), actionCollection(), "buy_estate");
//	buy_estate->setEnabled(false);

	createGUI();
	toolBar(0)->setBarPos(KToolBar::Left);

	wizard = 0;
	netw = new GameNetwork(this, "network");

	connect(netw, SIGNAL(msgError(QString)), this, SLOT(slotMsgError(QString)));
	connect(netw, SIGNAL(msgInfo(QString)), this, SLOT(slotMsgInfo(QString)));
	connect(netw, SIGNAL(msgStartGame(QString)), this, SLOT(slotMsgStartGame(QString)));
	connect(netw, SIGNAL(msgPlayerUpdate(QDomNode)), this, SLOT(slotMsgPlayerUpdate(QDomNode)));
	connect(netw, SIGNAL(msgEstateUpdate(QDomNode)), this, SLOT(slotMsgEstateUpdate(QDomNode)));

 	main = new QWidget(this, "main");
	main->show();

	layout = new QGridLayout(main, 8, 2);

	output = new QTextView(main, "output");
	output->setMinimumWidth(250);

	input = new QLineEdit(main, "input");
	connect(input, SIGNAL(returnPressed()), this, SLOT(slotSendMsg()));

	KMonopBoard *board = new KMonopBoard(netw, main, "board");

	layout->addWidget(output, 6, 0);
	layout->addWidget(input, 7, 0);
	layout->addMultiCellWidget(board, 0, 7, 1, 1);
	layout->setRowStretch(6, 1); // make board+output stretch, not the rest
	layout->setColStretch(1, 1); // make board stretch, not the rest

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
	output->append("ERR: " + msg);
}

void KMonop::slotMsgInfo(QString msg)
{
	output->append(msg);
}

void KMonop::slotMsgStartGame(QString msg)
{
	if (wizard!=0)
		wizard->hide();
		
	output->append("START: " + msg);
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

void KMonop::slotMsgEstateUpdate(QDomNode estateupdate)
{
	QDomAttr a_id, a_owner;
	QDomElement e;
	int id=0, owner=0;

	e = estateupdate.toElement();
	if(!e.isNull())
	{
		a_id = e.attributeNode(QString("id"));
		a_owner = e.attributeNode(QString("owner"));

		id = a_id.value().toInt();
		owner = a_owner.value().toInt() - 1;
		if (id < 40 && owner < MAXPLAYERS)
		{
			cout << "port " << owner << " id " << id << endl;
			port[owner]->setOwned(id, true);
		}
	}
}
