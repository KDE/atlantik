#include <qvgroupbox.h>
#include <qradiobutton.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
 
#include "selectconfiguration_widget.moc"

SelectConfiguration::SelectConfiguration(QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, 10);
	CHECK_PTR(m_mainLayout);

	// Player list.
	QVGroupBox *playerGroupBox;
	playerGroupBox = new QVGroupBox(i18n("Player list"), this, "groupBox");
	m_mainLayout->addWidget(playerGroupBox); 

	// Game configuration.
	QVGroupBox *groupBox;
	groupBox = new QVGroupBox(i18n("Game configuration"), this, "groupBox");
	m_mainLayout->addWidget(groupBox); 

	// List of  players
	m_playerList = new QListView(playerGroupBox, "m_playerList");
	m_playerList->addColumn(QString(i18n("Id")));
	m_playerList->addColumn(QString(i18n("Name")));
	m_playerList->addColumn(QString(i18n("Host")));
//	m_mainLayout->addWidget(m_playerList);

	connect(m_playerList, SIGNAL(clicked(QListViewItem *)), this, SLOT(validateConnectButton()));
	connect(m_playerList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(connectPressed()));
	connect(m_playerList, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)), this, SLOT(validateConnectButton()));
	connect(m_playerList, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(validateConnectButton()));

	// Add new configuration option to list view
//	QListViewItem *item = new QListViewItem(m_configurationList, i18n("Start a new configuration"), "");
//	item->setPixmap(0, SmallIcon("filenew"));

	m_connectButton = new QPushButton(SmallIcon("forward"), i18n("Start game"), this);
	m_connectButton->setEnabled(false);
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
	validateConnectButton();
//	emit statusChanged();
}

void SelectConfiguration::slotPlayerListAdd(QString playerId, QString name, QString host)
{
	QListViewItem *item = new QListViewItem(m_playerList, playerId, name, host);
	item->setPixmap(0, QPixmap(SmallIcon("personal")));

	validateConnectButton();
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
	validateConnectButton();
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
	validateConnectButton();
//	emit statusChanged();
}

void SelectConfiguration::validateConnectButton()
{
	if (m_playerList->selectedItem())
		m_connectButton->setEnabled(true);
	else
		m_connectButton->setEnabled(false);
}

void SelectConfiguration::connectPressed()
{
	if (QListViewItem *item = m_playerList->selectedItem())
	{
		if (int playerId = item->text(1).toInt())
			emit joinConfiguration(playerId);
		else
			emit newConfiguration();
	}

	KMessageBox::sorry(this, i18n(
		"The new game wizard is undergoing a rewrite which has not been finished yet.\n"
		"You cannot start a game at the moment."
		));
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
