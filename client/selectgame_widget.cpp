// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <qvgroupbox.h>
#include <qradiobutton.h>

#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>

#include "selectgame_widget.moc"

SelectGame::SelectGame(QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint());
	Q_CHECK_PTR(m_mainLayout);

	QVGroupBox *groupBox;
	groupBox = new QVGroupBox(i18n("Create or Select a monopd Game"), this, "groupBox");
	m_mainLayout->addWidget(groupBox);

	// List of games
	m_gameList = new KListView(groupBox, "m_gameList");
	m_gameList->addColumn(QString(i18n("Game")));
	m_gameList->addColumn(QString(i18n("Description")));
	m_gameList->addColumn(QString(i18n("Id")));
	m_gameList->addColumn(QString(i18n("Players")));
//	m_mainLayout->addWidget(m_gameList);

	connect(m_gameList, SIGNAL(clicked(QListViewItem *)), this, SLOT(validateConnectButton()));
	connect(m_gameList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(connectPressed()));
	connect(m_gameList, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)), this, SLOT(validateConnectButton()));
	connect(m_gameList, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(validateConnectButton()));

	QHBoxLayout *buttonBox = new QHBoxLayout(this, 0, KDialog::spacingHint());
	m_mainLayout->addItem(buttonBox);

	buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_connectButton = new KPushButton(SmallIcon("forward"), i18n("Create Game"), this);
	m_connectButton->setEnabled(false);
	buttonBox->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(pressed()), this, SLOT(connectPressed()));
	
    // Status indicator
	m_statusLabel = new QLabel(this);
	m_statusLabel->setText(i18n("Retrieving game list..."));
	m_mainLayout->addWidget(m_statusLabel);
}

void SelectGame::slotGameListClear()
{
	m_gameList->clear();
	validateConnectButton();
//	emit statusChanged();
}

void SelectGame::slotGameListAdd(QString gameId, QString name, QString description, QString players, QString gameType)
{
	if (gameId == "-1")
	{
		QListViewItem *item = new QListViewItem(m_gameList, i18n("Create a new %1 Game").arg(name), description, "", "", gameType);
		item->setPixmap(0, QPixmap(SmallIcon("filenew")));
	}
	else
	{
		QListViewItem *item = new QListViewItem(m_gameList, i18n("Join %1 Game #%2").arg(name).arg(gameId), description, gameId, players, gameType);
		item->setPixmap(0, QPixmap(SmallIcon("atlantik")));
	}

	validateConnectButton();
}

void SelectGame::slotGameListEdit(QString gameId, QString name, QString description, QString players, QString gameType)
{
	QListViewItem *item = m_gameList->firstChild();
	while (item)
	{
		if (item->text(2) == gameId)
		{
			if (!description.isEmpty())
				item->setText(1, description);
			item->setText(3, players);
			if (!gameType.isEmpty())
				item->setText(4, gameType);
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
	if (QListViewItem *item = m_gameList->selectedItem())
	{
		if (int gameId = item->text(2).toInt())
			m_connectButton->setText(i18n("Join Game"));
		else
			m_connectButton->setText(i18n("Create Game"));

		m_connectButton->setEnabled(true);
	}
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
			emit newGame(item->text(4));
	}
}
