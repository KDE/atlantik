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

	select_game = new SelectGame(this, "select_game");
	addPage(select_game, QString("Select or create a game:"));
	setHelpEnabled(select_game, false);

	configure_game = new ConfigureGame(this, "configure_game");
	addPage(configure_game, QString("Game configuration and list of players"));
	setHelpEnabled(configure_game, false);
	setFinishEnabled(configure_game, false);
}

NewGameWizard::~NewGameWizard()
{
}

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
		setNextEnabled(select_game, true);
	else
		setNextEnabled(select_game, false);
}

void NewGameWizard::slotInit(const QString &_name)
{
	cout << "page that should receive init: [" << _name << "]" << endl;
	if (title(select_server) == _name)
		cout << "that would be select_server" << endl;
	if (title(select_game) == _name)
	{
		cout << "that would be select_game" << endl;
		select_game->initPage();
	}
}

SelectGame::SelectGame(QWidget *parent, const char *name) : QWidget(parent, name)
{
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
	delete netw;

	status_label->setText("Connecting to server...");

	netw = new GameNetwork(this, "network");
	connect(netw, SIGNAL(connected()), this, SLOT(slotConnected()));
	connect(netw, SIGNAL(fetchedGameList(QDomNode)), this, SLOT(slotFetchedGameList(QDomNode)));
	connect(netw, SIGNAL(readyRead()), netw, SLOT(slotRead()));
	netw->connectToHost("localhost", 1234);
	netw->slotWrite(".gl");
}

bool SelectGame::validateNext()
{
	if (bnew->isChecked() || list->selectedItem())
		return true;
	else
		return false;
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

ConfigureGame::ConfigureGame(QWidget *parent, const char *name) : QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	CHECK_PTR(layout);

	status_label = new QLabel(this);
	status_label->setText("Configuration of the game is not yet supported by the monopd server.");
	layout->addWidget(status_label);
}

NewGameDialog::NewGameDialog(QWidget *parent, const char *name, bool modal) : KDialog(parent, name, modal)
{
	QVBoxLayout *qbox = new QVBoxLayout(this, marginHint(), spacingHint());
	CHECK_PTR(qbox);
	
	QLabel *header_label = new QLabel(this);
	/* steps: select server, connect, listgames, join/create, config/wait, play */
	header_label->setText("Please select a server to connect to:\n(you should choose localhost for now)\n");
	qbox->addWidget(header_label);

	list = new QListView(this);

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

	KButtonBox *bbox = new KButtonBox( this, KButtonBox::Horizontal, 0, spacingHint());
	CHECK_PTR( bbox );
	qbox->addWidget(bbox);

	bbox->addStretch();

	bconnect = bbox->addButton( i18n("&Connect"), false );
	bcancel = bbox->addButton( i18n("C&ancel"), false );

	bbox->layout();
	bconnect->setDefault(true);
	
	connect(bconnect, SIGNAL(clicked()), this, SLOT(slotConnect()));
	connect(bcancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
}

NewGameDialog::~NewGameDialog()
{
}

void NewGameDialog::slotConnect()
{
	cout << "bconnect clicked" << endl;
	QListViewItem *item;
	if ((item = list->selectedItem()))
	{
		KMessageBox::sorry(0, QString("Network code has not yet been completed."), "Action unsupported");
	}
	else
		KMessageBox::sorry(0, QString(i18n("You did not select a server to connect to.")), "Select a server");
}

void NewGameDialog::slotCancel()
{
	cout << "bcancel clicked" << endl;
	hide();
}
