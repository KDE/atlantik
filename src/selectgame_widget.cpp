#include <qvgroupbox.h>
#include <qradiobutton.h>

#include <klocale.h>
#include <kiconloader.h>
 
#include "selectgame_widget.moc"

SelectGame::SelectGame(QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, 10);
	CHECK_PTR(m_mainLayout);

	QVGroupBox *groupBox;
	groupBox = new QVGroupBox(i18n("Start or select a monopd game"), this, "groupBox");
	m_mainLayout->addWidget(groupBox);

	// List of games
	m_gameList = new QListView(groupBox, "m_gameList");
	m_gameList->addColumn(QString(i18n("Description")));
	m_gameList->addColumn(QString(i18n("Id")));
	m_gameList->addColumn(QString(i18n("Players")));
//	m_mainLayout->addWidget(m_gameList);

	connect(m_gameList, SIGNAL(clicked(QListViewItem *)), this, SLOT(validateConnectButton()));
	connect(m_gameList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(connectPressed()));
	connect(m_gameList, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)), this, SLOT(validateConnectButton()));
	connect(m_gameList, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(validateConnectButton()));

	// Add default new game option to list view
	QListViewItem *item = new QListViewItem(m_gameList, i18n("Start a new game"), "", "");
	item->setPixmap(0, SmallIcon("filenew"));

	m_connectButton = new QPushButton(SmallIcon("forward"), i18n("Connect"), this);
	m_connectButton->setEnabled(false);
	m_mainLayout->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(pressed()), this, SLOT(connectPressed()));
	
    // Status indicator
	status_label = new QLabel(this);
	status_label->setText(i18n("Retrieving game list..."));
	m_mainLayout->addWidget(status_label);
}

void SelectGame::slotGameListClear()
{
	m_gameList->clear();

	// Add default new game option to list view
	QListViewItem *item = new QListViewItem(m_gameList, i18n("Start a new game"), "", "");
	item->setPixmap(0, SmallIcon("filenew"));

	validateConnectButton();
//	emit statusChanged();
}

void SelectGame::slotGameListAdd(QString gameId, QString description, QString players)
{
	QListViewItem *item = new QListViewItem(m_gameList, description, gameId, players);
	item->setPixmap(0, QPixmap(SmallIcon("atlantik")));

	validateConnectButton();
}

void SelectGame::slotGameListEdit(QString gameId, QString description, QString players)
{
	QListViewItem *item = m_gameList->firstChild();
	while (item)
	{
		if (item->text(0) == gameId)
		{
			item->setText(1, description);
			item->setText(2, players);
			m_gameList->triggerUpdate();
			return;
		}
		item = item->nextSibling();
	}
	validateConnectButton();
//	emit statusChanged();
}

void SelectGame::slotGameListDel(QString gameId)
{
	QListViewItem *item = m_gameList->firstChild();
	while (item)
	{
		if (item->text(1) == gameId)
		{
			delete item;
			return;
		}
		item = item->nextSibling();
	}
	validateConnectButton();
//	emit statusChanged();
}

void SelectGame::validateConnectButton()
{
	if (m_gameList->selectedItem())
		m_connectButton->setEnabled(true);
	else
		m_connectButton->setEnabled(false);
}

void SelectGame::connectPressed()
{
	if (QListViewItem *item = m_gameList->selectedItem())
	{
		if (int gameId = item->text(1).toInt())
			emit joinGame(gameId);
		else
			emit newGame();
	}
}
