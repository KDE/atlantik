#include <qradiobutton.h>

#include <kdebug.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
 
#include "selectconfiguration_widget.moc"

SelectConfiguration::SelectConfiguration(QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, 10);
	CHECK_PTR(m_mainLayout);

	// Player list.
	m_playerGroupBox = new QVGroupBox(i18n("Player list"), this, "groupBox");
	m_mainLayout->addWidget(m_playerGroupBox); 

	// Game configuration.
	m_groupBox = new QVGroupBox(i18n("Game configuration"), this, "groupBox");
	m_mainLayout->addWidget(m_groupBox); 

	// List of  players
	m_playerList = new KListView(m_playerGroupBox, "m_playerList");
	m_playerList->addColumn(QString(i18n("Id")));
	m_playerList->addColumn(QString(i18n("Name")));
	m_playerList->addColumn(QString(i18n("Host")));
//	m_mainLayout->addWidget(m_playerList);

	connect(m_playerList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(connectPressed()));

	// Add new configuration option to list view
//	QListViewItem *item = new QListViewItem(m_configurationList, i18n("Start a new configuration"), "");
//	item->setPixmap(0, SmallIcon("filenew"));

	m_connectButton = new KPushButton(SmallIcon("forward"), i18n("Start game"), this);
	m_mainLayout->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(pressed()), this, SLOT(connectPressed()));
	
    // Status indicator
	status_label = new QLabel(this);
	status_label->setText(i18n("Retrieving configuration list..."));
	m_mainLayout->addWidget(status_label);
}

void SelectConfiguration::slotPlayerListClear()
{
	m_playerList->clear();
//	emit statusChanged();
}

void SelectConfiguration::slotPlayerListAdd(QString playerId, QString name, QString host)
{
	kdDebug() << "SelectConfiguration::slotPlayerListAdd" << endl;
	QListViewItem *item = new QListViewItem(m_playerList, playerId, name, host);
	item->setPixmap(0, QPixmap(SmallIcon("personal")));
}

void SelectConfiguration::slotPlayerListEdit(QString playerId, QString name, QString host)
{
	QListViewItem *item = m_playerList->firstChild();
	while (item)
	{
		if (item->text(0) == playerId)
		{
			item->setText(1, name);
			item->setText(2, host);
			m_playerList->triggerUpdate();
			return;
		}
		item = item->nextSibling();
	}
//	emit statusChanged();
}

void SelectConfiguration::slotPlayerListDel(QString playerId)
{
	QListViewItem *item = m_playerList->firstChild();
	while (item)
	{
		if (item->text(0) == playerId)
		{
			delete item;
			return;
		}
		item = item->nextSibling();
	}
//	emit statusChanged();
}

void SelectConfiguration::connectPressed()
{
	emit startGame();
	return;

	if (QListViewItem *item = m_playerList->selectedItem())
	{
		if (int playerId = item->text(1).toInt())
			emit joinConfiguration(playerId);
		else
			emit newConfiguration();
	}

	m_messageBox = new QVGroupBox(i18n("Sorry"), this, "messageBox");
	m_mainLayout->addWidget(m_messageBox); 

	QLabel *label = new QLabel(m_messageBox);
	label->setText(i18n(
		"The new game wizard is undergoing a rewrite which has not been finished yet.\n"
		"You cannot start a game at the moment."
		));

	KPushButton *button = new KPushButton(i18n("Ok"), m_messageBox, "button");

	m_playerGroupBox->setEnabled(false);
	m_groupBox->setEnabled(false);
	m_connectButton->setEnabled(false);
	status_label->setEnabled(false);

	connect(button, SIGNAL(clicked()), this, SLOT(slotClicked()));

	m_messageBox->show();
}

void SelectConfiguration::slotClicked()
{
	delete m_messageBox;
	m_playerGroupBox->setEnabled(true);
	m_groupBox->setEnabled(true);
	m_connectButton->setEnabled(true);
	status_label->setEnabled(true);
}

/*
// some old code i might want to reuse

	// No i18n, this is not a permanent label anyway.
	status_label->setText("Configuration of the game is not yet supported by the monopd server.\nGames will be played using the standard rules.");

void ConfigureGame::slotPlayerlistEndUpdate(QString type)
{
	if (type=="full")
		status_label->setText(i18n("Fetched list of players."));

	emit statusChanged();
}

bool ConfigureGame::validateNext()
{
	if (list->childCount() >= 2)
		return true;
	else
		return false;
}
*/
