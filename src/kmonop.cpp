#include <qlayout.h>
#include <qlineedit.h>
#include <iostream.h>

#include <kstdaction.h>
#include <kaction.h>
#include <kapp.h>

#include "kmonop.moc"
#include "portfolioview.h"
#include "board.h"

KMonop::KMonop (const char *name) :
  KTMainWindow (name)
{
  KStdAction::openNew(this, SLOT(slotNewGame()), actionCollection(), "game_new");
  KStdAction::quit(kapp, SLOT(closeAllWindows()), actionCollection(), "game_quit");

  createGUI();

	netw = new GameNetwork(this, "network");

	connect(netw, SIGNAL(msgError(QString)), this, SLOT(slotMsgError(QString)));
	connect(netw, SIGNAL(msgStartGame(QString)), this, SLOT(slotMsgStartGame(QString)));

  QWidget *main = new QWidget(this, "main");
  PortfolioView *port = new PortfolioView(main);
  PortfolioView *port2 = new PortfolioView(main);
  port->setName("Player 1");
  port2->setName("Player 2");
  port2->hide();
  main->show();

  QGridLayout *layout = new QGridLayout(main, 4, 3);

  output = new QTextView(main, "output");
  output->setMinimumHeight(70);
  server = new QLineEdit(main, "server");
  KMonopBoard *board = new KMonopBoard(main, "board");
  layout->addWidget(port, 0, 0);
  layout->addWidget(port2, 1, 0);
  layout->addWidget(output, 2, 0);
  layout->addMultiCellWidget(board, 0, 3, 1, 1);
  layout->setRowStretch(2, 1); // make board+output stretch, not the rest
  layout->setColStretch(1, 1); // make board stretch, not the rest
  layout->addWidget(server, 3, 0);
//  sock = new QSocket(this, "mysock");
//  sock->connectToHost("localhost", 1234);
//  connect(sock, SIGNAL(connected()), this, SLOT(slotConnected()));
//  connect(sock, SIGNAL(readyRead()), this, SLOT(slotRead()));
//  connect(server, SIGNAL(returnPressed()), this, SLOT(slotWrite()));
  setView(main);
}

void KMonop::slotConnected()
{
	output->append("we are connected\n");
	cout << "wow, we are connected. cheer!" << endl;
	statusBar()->message("connected..", 2000);
}

void KMonop::slotRead()
{
	QString str;
	cout << "wow, we can read. cheer!\n" << endl;

	while(sock->canReadLine())
	{
		str = sock->readLine();
		output->append(str);
		cout << "[[[ " + str + " ]]] " << endl;
	}

	cout << "no more complete lines" << endl;
}

void KMonop::slotWrite()
{
	QString str(server->text());
	str.append("\n");
	output->append(str);
	sock->writeBlock(str.latin1(), strlen(str.latin1()));
	server->setText("");
}

void KMonop::slotNewGame()
{
	int result;

	wizard = new NewGameWizard(netw, this, "newgame", 1);
	result = wizard->exec();
	delete wizard;
}

void KMonop::slotMsgError(QString msg)
{
	output->append("ERROR: ");
	output->append(msg);
}

void KMonop::slotMsgStartGame(QString msg)
{
	output->append("START: ");
	output->append(msg);
	wizard->hide();
}
