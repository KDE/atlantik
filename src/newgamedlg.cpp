#include <stdlib.h>

#include <qlayout.h>
#include <iostream.h>
#include <qlabel.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>

#include <kbuttonbox.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "newgamedlg.moc"
 
NewGameWizard::NewGameWizard(QWidget *parent, const char *name, bool modal, WFlags f) : KWizard( parent, name, modal, f)
{
	// Initiate network connection
	// TODO: Should eventually go into KMonop..
	netw = new GameNetwork(this, "network");

	// Select server page
	// TODO: Turn into seperate class..
	select_server = new QWidget(this);
	QVBoxLayout *qbox = new QVBoxLayout(select_server);
	CHECK_PTR(qbox);
	
	list = new QListView(select_server);

	QListViewItem *item;
	item = new QListViewItem(list, "localhost", "1234", "0");
//	item = new QListViewItem(list, "monopd.capsi.com", "1234", "0");

	QString column;
	column.setLatin1("Server", strlen("Server"));
	list->addColumn(column);
	column.setLatin1("Port", strlen("Port"));
	list->addColumn(column);
	column.setLatin1("Users", strlen("Users"));
	list->addColumn(column);

	qbox->addWidget(list);

	// Belongs in select_server class
	connect(list, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(slotListClick()));
	connect(list, SIGNAL(clicked(QListViewItem *)), this, SLOT(slotListClick()));
	connect(list, SIGNAL(pressed(QListViewItem *)), this, SLOT(slotListClick()));

	// Belongs here
	connect(this, SIGNAL(selected(const QString &)), this, SLOT(slotInit(const QString &)));

	addPage(select_server, QString("Select a game server to connect to:"));
	setHelpEnabled(select_server, false);
	setNextEnabled(select_server, false);


	// Select game page
	select_game = new SelectGame(netw, this, "select_game");

	addPage(select_game, QString("Select or create a game:"));
	setHelpEnabled(select_game, false);

	connect(netw, SIGNAL(connected()), select_game, SLOT(slotConnected()));
	connect(netw, SIGNAL(fetchedGameList(QDomNode)), select_game, SLOT(slotFetchedGameList(QDomNode)));

	// Configure game page
	configure_game = new ConfigureGame(netw, this, "configure_game");
	configure_game->setGameId(select_game->gameToJoin());

	addPage(configure_game, QString("Game configuration and list of players"));
	setHelpEnabled(configure_game, false);
	setFinishEnabled(configure_game, false);

	connect(netw, SIGNAL(fetchedPlayerList(QDomNode)), configure_game, SLOT(slotFetchedPlayerList(QDomNode)));
}

NewGameWizard::~NewGameWizard()
{
}

// Integrate with slotValidateNext()
void NewGameWizard::slotListClick()
{
	if (list->selectedItem())
		setNextEnabled(select_server, true);
	else
		setNextEnabled(select_server, false);
}

void NewGameWizard::slotValidateNext()
{
	if (select_game->validateNext())
	{
		setNextEnabled(select_game, true);
		configure_game->setGameId(select_game->gameToJoin());
	}
	else
		setNextEnabled(select_game, false);
}

void NewGameWizard::slotInit(const QString &_name)
{
	cout << "initPage: " << _name << endl;
//	if (title(select_server) == _name)
//		select_server->initPage();
	if (title(select_game) == _name)
		select_game->initPage();
	if (title(configure_game) == _name)
		configure_game->initPage();
}

SelectGame::SelectGame(GameNetwork *_nw, QWidget *parent, const char *name) : QWidget(parent, name)
{
	netw = _nw;

	QVBoxLayout *layout = new QVBoxLayout(this);
	CHECK_PTR(layout);

	QVButtonGroup *bgroup = new QVButtonGroup(this);
	bgroup->setExclusive(true);

	bnew = new QRadioButton(QString("Create a new game"), bgroup, "bnew");
	bnew->setChecked(true);
	connect(bnew, SIGNAL(stateChanged(int)), parent, SLOT(slotValidateNext()));

	bjoin = new QRadioButton(QString("Join a game"), bgroup, "bjoin");
	bjoin->setChecked(false);
	connect(bjoin, SIGNAL(stateChanged(int)), parent, SLOT(slotValidateNext()));
	
	layout->addWidget(bgroup);

	list = new QListView(this);

	list->addColumn(QString("Id"));
	list->addColumn(QString("Players"));
	list->addColumn(QString("Description"));

	connect(list, SIGNAL(selectionChanged(QListViewItem *)), parent, SLOT(slotValidateNext()));
	connect(list, SIGNAL(clicked(QListViewItem *)), parent, SLOT(slotValidateNext()));
	connect(list, SIGNAL(pressed(QListViewItem *)), parent, SLOT(slotValidateNext()));

	layout->addWidget(list);
	
	status_label = new QLabel(this);
	status_label->setText("Connecting to server...");
	layout->addWidget(status_label);
}

void SelectGame::initPage()
{
	status_label->setText("Connecting to server...");

	// TODO: Replace with host from select_server list
	netw->connectToHost("localhost", 1234);

	// TODO: What if connection cannot be made?

	// Fetch list of games
	netw->writeData(".gl");
}

bool SelectGame::validateNext()
{
	if (bnew->isChecked() || list->selectedItem())
		return true;
	else
		return false;
}

QString SelectGame::gameToJoin() const
{
	if (bnew->isChecked())
		return QString("0");
	else if (QListViewItem *item = list->selectedItem())
		return item->text(0);
	else
		return QString("0");
}

void SelectGame::slotConnected()
{
	status_label->setText(QString("Connected. Fetching list of games..."));
}

void SelectGame::slotFetchedGameList(QDomNode gamelist)
{
	QDomAttr a;
	QDomNode n = gamelist.firstChild();
	QListViewItem *item;

	list->clear();

	while(!n.isNull())
	{
		QDomElement e = n.toElement();
		if(!e.isNull())
		{
			if (e.tagName() == "game")
			{
				item =  new QListViewItem(list, e.attributeNode(QString("id")).value(), e.attributeNode(QString("players")).value());
				list->triggerUpdate();
			}
		}
		n = n.nextSibling();
	}

	status_label->setText(QString("Fetched list of games."));
}

ConfigureGame::ConfigureGame(GameNetwork *_nw, QWidget *parent, const char *name) : QWidget(parent, name)
{
	netw = _nw;
	game_id = QString("0");

	QVBoxLayout *layout = new QVBoxLayout(this);
	CHECK_PTR(layout);

	list = new QListView(this);

	list->addColumn(QString("Player"));
	list->addColumn(QString("Host"));

	layout->addWidget(list);

	status_label = new QLabel(this);
	status_label->setText("Configuration of the game is not yet supported by the monopd server.");
	layout->addWidget(status_label);
}

void ConfigureGame::initPage()
{
	if (game_id == "0")
	{
		// Create a new game
		netw->writeData(".gn");
	}
	else
	{
		// Join existing game
		QString str(".gj");
		str.append(game_id);
		netw->writeData(str.latin1());
	}
	
	// Fetch playerlist
	netw->writeData(".gp");
}

void ConfigureGame::setGameId(const QString &_id)
{
	game_id = _id;
}

void ConfigureGame::slotFetchedPlayerList(QDomNode gamelist)
{
	QDomAttr a;
	QDomNode n = gamelist.firstChild();
	QListViewItem *item;

	list->clear();

	while(!n.isNull())
	{
		QDomElement e = n.toElement();
		if(!e.isNull())
		{
			if (e.tagName() == "player")
			{
				item =  new QListViewItem(list, e.attributeNode(QString("name")).value());
				list->triggerUpdate();
			}
		}
		n = n.nextSibling();
	}

	status_label->setText(QString("Fetched list of players."));
}
