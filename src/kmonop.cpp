#include <qlineedit.h>
#include <iostream.h>

#include <kstdaction.h>
#include <kaction.h>
#include <kapp.h>

#include "kmonop.moc"
#include "board.h"

KMonop::KMonop (const char *name) :
  KTMainWindow (name)
{
	KStdAction::openNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
	KStdAction::quit(kapp, SLOT(closeAllWindows()), actionCollection(), "game_quit");

	createGUI();

	wizard = 0;
	netw = new GameNetwork(this, "network");

	connect(netw, SIGNAL(msgError(QString)), this, SLOT(slotMsgError(QString)));
	connect(netw, SIGNAL(msgStartGame(QString)), this, SLOT(slotMsgStartGame(QString)));
	connect(netw, SIGNAL(msgPlayerList(QDomNode)), this, SLOT(slotMsgPlayerList(QDomNode)));

 	main = new QWidget(this, "main");
	main->show();

	layout = new QGridLayout(main, 8, 2);

//	Token *token = new Token(main, "token");
//	layout->addWidget(token, 1, 0);

  output = new QTextView(main, "output");
  output->setMinimumHeight(70);
  input = new QLineEdit(main, "input");
  KMonopBoard *board = new KMonopBoard(main, "board");
  layout->addWidget(output, 6, 0);
  layout->addMultiCellWidget(board, 0, 7, 1, 1);
  layout->setRowStretch(6, 1); // make board+output stretch, not the rest
  layout->setColStretch(1, 1); // make board stretch, not the rest
  layout->addWidget(input, 7, 0);

  setView(main);
}

void KMonop::slotNewGame()
{
	int result;

	wizard = new NewGameWizard(netw, this, "newgame", 1);
	result = wizard->exec();
	netw->writeData(".gs");
	delete wizard;
	wizard = 0;
}

void KMonop::slotMsgError(QString msg)
{
	output->append("ERROR: ");
	output->append(msg);
}

void KMonop::slotMsgStartGame(QString msg)
{
	if (wizard!=0)
		wizard->hide();
	else
		cout << "wizard already gone? odd" << endl;
		
	output->append("START: ");
	output->append(msg);
}

void KMonop::slotMsgPlayerList(QDomNode playerlist)
{
	QDomAttr a;
	QDomElement e;
	
	// Only process final playerlist.
	e = playerlist.toElement();
	if(!e.isNull())
	{
		a = e.attributeNode(QString("final"));

		if(a.isNull() || a.value() == "false")
			return;
	}
	else
		return;

	QDomNode n = playerlist.firstChild();
	QListViewItem *item;
	int i=0;

	// Maximum of six players, right? Check with monopd, possibly fetch
	// number first and make portfolio overviews part of QHLayout to allow
	// any theoretically number.
	while(!n.isNull() && i<6)
	{
		e = n.toElement();
		if(!e.isNull())
		{
			if (e.tagName() == "player")
			{
				port[i] = new PortfolioView(main);
				port[i]->setName(e.attributeNode(QString("name")).value());
				port[i]->show();
				layout->addWidget(port[i], i, 0);
				i++;
			}
		}
		n = n.nextSibling();
	}
}
