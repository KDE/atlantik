#include <qvgroupbox.h>
#include <qradiobutton.h>
#include <qhbox.h>

#include <kdebug.h>
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
	m_gameList = new KListView(groupBox, "m_gameList");
	m_gameList->addColumn(QString(i18n("Description")));
	m_gameList->addColumn(QString(i18n("Game type")));
	m_gameList->addColumn(QString(i18n("Id")));
	m_gameList->addColumn(QString(i18n("Players")));
//	m_mainLayout->addWidget(m_gameList);

	connect(m_gameList, SIGNAL(clicked(QListViewItem *)), this, SLOT(validateConnectButton()));
	connect(m_gameList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(connectPressed()));
	connect(m_gameList, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)), this,
 SLOT(validateConnectButton()));
	connect(m_gameList, SIGNAL(selectionChanged(QListViewItem *)), this,
 SLOT(validateConnectButton()));

	QHBox *buttonBox = new QHBox(this);
	m_mainLayout->addWidget(buttonBox);

	m_connectButton = new KPushButton(SmallIcon("forward"), i18n("Connect"), buttonBox);
	m_connectButton->setEnabled(false);
//	m_mainLayout->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(pressed()), this, SLOT(connectPressed()));
	
    // Status indicator
	status_label = new QLabel(this);
	status_label->setText(i18n("Retrieving game list..."));
	m_mainLayout->addWidget(status_label);
}

void SelectGame::slotGameListClear()
{
	m_gameList->clear();
	validateConnectButton();
//	emit statusChanged();
}

void SelectGame::slotGameListAdd(QString gameId, QString gameType, QString description, QString players)
{
	if (gameId == "-1")
	{
		QListViewItem *item = new QListViewItem(m_gameList, i18n("Start a new %1 game").arg(description), gameType, "", "");
		item->setPixmap(0, QPixmap(SmallIcon("filenew")));
	}
	else
	{
		QListViewItem *item = new QListViewItem(m_gameList, description, gameType, gameId, players);
		item->setPixmap(0, QPixmap(SmallIcon("atlantik")));
	}

	validateConnectButton();
}

void SelectGame::slotGameListEdit(QString gameId, QString gameType, QString description, QString
 players)
{
	QListViewItem *item = m_gameList->firstChild();
	while (item)
	{
		if (item->text(2) == gameId)
		{
			item->setText(1, description);
			item->setText(2, gameType);
			item->setText(3, players);
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
		if (item->text(2) == gameId)
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
		if (int gameId = item->text(2).toInt())
			emit joinGame(gameId);
		else
			emit newGame(item->text(1));
	}
}
