#include <qlayout.h>
#warning remove iostream output
#include <iostream.h>
#include <qlabel.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>

#include <kbuttonbox.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "newgamedlg.moc"
 
NewGameWizard::NewGameWizard(QWidget *parent, const char *name, bool modal, WFlags f) : KWizard(parent, name, modal, f)
{
	// Select server page
	select_server = new SelectServer(this, "select_server");

	addPage(select_server, QString("Select a game server to connect to:"));
	setHelpEnabled(select_server, false);
	setNextEnabled(select_server, false);

	// Select game page
	select_game = new SelectGame(this, "select_game");
	select_game->setGameHost(select_server->hostToConnect());
	connect(select_game, SIGNAL(statusChanged()), this, SLOT(slotValidateNext()));

	addPage(select_game, QString("Select or create a game:"));
	setHelpEnabled(select_game, false);
	setNextEnabled(select_game, false);

	// Configure game page
	configure_game = new ConfigureGame(this, "configure_game");
	configure_game->setGameId(select_game->gameToJoin());
	connect(gameNetwork, SIGNAL(clearPlayerList()), configure_game, SLOT(slotClearPlayerList()));
	connect(gameNetwork, SIGNAL(addToPlayerList(QString, QString)), configure_game, SLOT(slotAddToPlayerList(QString, QString)));

	addPage(configure_game, QString("Game configuration and list of players"));
	setHelpEnabled(configure_game, false);
	setFinishEnabled(configure_game, false);

	// General
	connect(this, SIGNAL(selected(const QString &)), this, SLOT(slotInit(const QString &)));
}

NewGameWizard::~NewGameWizard()
{
}

void NewGameWizard::slotValidateNext()
{
	// TODO: different slots for different pages
	// or: passing widget pointer to slot and evaluate

	if (select_server->validateNext())
	{
		setNextEnabled(select_server, true);
		select_game->setGameHost(select_server->hostToConnect());
	}
	else
		setNextEnabled(select_server, false);

	if (select_game->validateNext())
	{
		setNextEnabled(select_game, true);
		configure_game->setGameId(select_game->gameToJoin());
	}
	else
		setNextEnabled(select_game, false);

	if (configure_game->validateNext())
		setFinishEnabled(configure_game, true);
	else
		setFinishEnabled(configure_game, false);
}

void NewGameWizard::slotInit(const QString &_name)
{
	cout << "initPage: " << _name << endl;
#warning create SelectServer::initPage();
//	if (title(select_server) == _name)
//		select_server->initPage();
	if (title(select_game) == _name)
		select_game->initPage();
	if (title(configure_game) == _name)
		configure_game->initPage();
}

SelectServer::SelectServer(QWidget *parent, const char *name) : QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	CHECK_PTR(layout);

	// List of servers
	list = new QListView(this);
	list->addColumn(QString("Server"));
	list->addColumn(QString("Port"));
	list->addColumn(QString("Users"));
	connect(list, SIGNAL(selectionChanged(QListViewItem *)), parent, SLOT(slotValidateNext()));
	connect(list, SIGNAL(clicked(QListViewItem *)), parent, SLOT(slotValidateNext()));
	connect(list, SIGNAL(pressed(QListViewItem *)), parent, SLOT(slotValidateNext()));
	layout->addWidget(list);

	// Until Monopigator works, add some servers manually
	QListViewItem *item;
	item = new QListViewItem(list, "localhost", "1234", "0");
	item = new QListViewItem(list, "monopd.capsi.com", "1234", "0");
}

bool SelectServer::validateNext()
{
	if (list->selectedItem())
		return true;
	else
		return false;
}

QString SelectServer::hostToConnect() const
{
	if (QListViewItem *item = list->selectedItem())
		return item->text(0);
	else
		return QString("0");
}

SelectGame::SelectGame(QWidget *parent, const char *name) : QWidget(parent, name)
{
	// Network interface
	connect(gameNetwork, SIGNAL(connected()), this, SLOT(slotConnected()));
	connect(gameNetwork, SIGNAL(gamelistUpdate(QString)), this, SLOT(slotGamelistUpdate(QString)));
	connect(gameNetwork, SIGNAL(gamelistEndUpdate(QString)), this, SLOT(slotGamelistEndUpdate(QString)));
	connect(gameNetwork, SIGNAL(gamelistAdd(QString, QString)), this, SLOT(slotGamelistAdd(QString, QString)));
	connect(gameNetwork, SIGNAL(gamelistDel(QString)), this, SLOT(slotGamelistDel(QString)));

	QVBoxLayout *layout = new QVBoxLayout(this);
	CHECK_PTR(layout);

	QVButtonGroup *bgroup = new QVButtonGroup(this);
	bgroup->setExclusive(true);
	layout->addWidget(bgroup);

	// Button to create new game
	bnew = new QRadioButton(QString("Create a new game"), bgroup, "bnew");
	connect(bnew, SIGNAL(stateChanged(int)), parent, SLOT(slotValidateNext()));

	// Button to join game
	bjoin = new QRadioButton(QString("Join a game"), bgroup, "bjoin");
	connect(bjoin, SIGNAL(stateChanged(int)), parent, SLOT(slotValidateNext()));

	// List of games
	list = new QListView(this);
	list->addColumn(QString("Id"));
	list->addColumn(QString("Players"));
	list->addColumn(QString("Description"));
	connect(list, SIGNAL(selectionChanged(QListViewItem *)), parent, SLOT(slotValidateNext()));
	connect(list, SIGNAL(clicked(QListViewItem *)), parent, SLOT(slotValidateNext()));
	connect(list, SIGNAL(pressed(QListViewItem *)), parent, SLOT(slotValidateNext()));
	layout->addWidget(list);

	// Status indicator
	status_label = new QLabel(this);
	status_label->setText("Connecting to server...");
	layout->addWidget(status_label);
}
 
void SelectGame::initPage()
{
	validateButtons();
	bnew->setChecked(true);

	status_label->setText("Connecting to server...");

	// TODO: Only connect when no connection is made yet, only fetch when
	// connection is already made.
	gameNetwork->connectToHost(gameHost, 1234);

	// TODO: What if connection cannot be made?

	// Fetch list of games
	gameNetwork->writeData(".gl");
}

void SelectGame::setGameHost(const QString &_host)
{
	gameHost = _host;
}

void SelectGame::validateButtons()
{
	bnew->setEnabled( (gameNetwork->state()==QSocket::Connection) ? true : false);

	if (list->childCount() > 0)
	{
		bjoin->setEnabled(true);
	}
	else
	{
		bjoin->setEnabled(false);
		list->clearSelection();
	}
}

bool SelectGame::validateNext()
{
	validateButtons();

	if ( (bnew->isEnabled() && bnew->isChecked()) || (bjoin->isEnabled() && bjoin->isChecked() && list->selectedItem()) )
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
	status_label->setText(QString("Connected."));
	emit statusChanged();
}

void SelectGame::slotGamelistUpdate(QString type)
{
	if (type=="full")
	{
		status_label->setText(QString("Fetching list of games..."));
		list->clear();
	}
}

void SelectGame::slotGamelistAdd(QString id, QString players)
{
	new QListViewItem(list, id, players);
	list->triggerUpdate();
}

void SelectGame::slotGamelistDel(QString id)
{
	QListViewItem *item = list->firstChild();
	while (item)
	{
		if (item->text(0) == id)
		{
			delete item;
			return;
		}
		item = item->nextSibling();
	}
}

void SelectGame::slotGamelistEndUpdate(QString type)
{
	if (type=="full")
		status_label->setText(QString("Fetched list of games."));

	emit statusChanged();
}

void SelectGame::slotInitPage()
{
	initPage();
}

ConfigureGame::ConfigureGame(QWidget *parent, const char *name) : QWidget(parent, name)
{
	game_id = QString("0");

	connect(this, SIGNAL(playerListChanged()), parent, SLOT(slotValidateNext()));

	QVBoxLayout *layout = new QVBoxLayout(this);
	CHECK_PTR(layout);

	list = new QListView(this);

	list->addColumn(QString("Player"));
	list->addColumn(QString("Host"));

	layout->addWidget(list);

	status_label = new QLabel(this);
	status_label->setText("Configuration of the game is not yet supported by the monopd server.\nGames will be played using the standard rules.");
	layout->addWidget(status_label);
}

void ConfigureGame::initPage()
{
	if (game_id == "0")
	{
		// Create a new game
		gameNetwork->writeData(".gn");
	}
	else
	{
		// Join existing game
		QString str(".gj");
		str.append(game_id);
		gameNetwork->writeData(str.latin1());
	}
}

void ConfigureGame::setGameId(const QString &_id)
{
	game_id = _id;
}

void ConfigureGame::slotClearPlayerList()
{
	list->clear();
}

void ConfigureGame::slotAddToPlayerList(QString name, QString host)
{
	new QListViewItem(list, name, host);
	list->triggerUpdate();

	emit playerListChanged();
//	status_label->setText(QString("Fetched list of players."));
}

bool ConfigureGame::validateNext()
{
	if (list->childCount() >= 2)
		return true;
	else
		return false;
}
