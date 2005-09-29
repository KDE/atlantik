// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include <qvgroupbox.h>
#include <qradiobutton.h>

#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <knotifyclient.h>

#include <atlantic_core.h>
#include <game.h>
#include <player.h>

#include "selectgame_widget.h"

SelectGame::SelectGame(AtlanticCore *atlanticCore, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_atlanticCore = atlanticCore;

	connect(m_atlanticCore, SIGNAL(createGUI(Game *)), this, SLOT(addGame(Game *)));
	connect(m_atlanticCore, SIGNAL(removeGUI(Game *)), this, SLOT(delGame(Game *)));

	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint());
	Q_CHECK_PTR(m_mainLayout);

	QVGroupBox *groupBox;
	groupBox = new QVGroupBox(i18n("Create or Select monopd Game"), this, "groupBox");
	m_mainLayout->addWidget(groupBox);

	// List of games
	m_gameList = new KListView(groupBox, "m_gameList");
	m_gameList->addColumn(i18n("Game"));
	m_gameList->addColumn(i18n("Description"));
	m_gameList->addColumn(i18n("Id"));
	m_gameList->addColumn(i18n("Players"));
	m_gameList->setAllColumnsShowFocus(true);
//	m_mainLayout->addWidget(m_gameList);

	connect(m_gameList, SIGNAL(clicked(QListViewItem *)), this, SLOT(validateConnectButton()));
	connect(m_gameList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(connectClicked()));
	connect(m_gameList, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)), this, SLOT(validateConnectButton()));
	connect(m_gameList, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(validateConnectButton()));

	QHBoxLayout *buttonBox = new QHBoxLayout(m_mainLayout, KDialog::spacingHint());

	KPushButton *backButton = new KPushButton(SmallIcon("back"), i18n("Server List"), this);
	buttonBox->addWidget(backButton);

	connect(backButton, SIGNAL(clicked()), this, SIGNAL(leaveServer()));

	buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_connectButton = new KPushButton(SmallIconSet("forward"), i18n("Create Game"), this);
	m_connectButton->setEnabled(false);
	buttonBox->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));

}

void SelectGame::addGame(Game *game)
{
	connect(game, SIGNAL(changed(Game *)), this, SLOT(updateGame(Game *)));

	if (game->id() == -1)
	{
		QListViewItem *item = new QListViewItem( m_gameList, i18n("Create a new %1 Game").arg(game->name()), game->description(), QString::null, QString::null, game->type() );
		item->setPixmap(0, QPixmap(SmallIcon("filenew")));
	}
	else
	{
		Player *master = game->master();
		QListViewItem *item = new QListViewItem( m_gameList, i18n("Join %1's %2 Game").arg( (master ? master->name() : QString::null), game->name() ), game->description(), QString::number(game->id()), QString::number(game->players()), game->type() );
		item->setPixmap( 0, QPixmap(SmallIcon("atlantik")) );
		item->setEnabled(game->canBeJoined());

		KNotifyClient::event(winId(), "newgame");

		connect(master, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
	}

//	validateConnectButton();
}

void SelectGame::delGame(Game *game)
{
	QListViewItem *item = findItem(game);
	if (!item)
		return;

	delete item;

	validateConnectButton();
}

void SelectGame::updateGame(Game *game)
{
	QListViewItem *item = findItem(game);
	if (!item)
		return;

	item->setText( 1, game->description() );

	if (game->id() == -1)
		item->setText(0, i18n("Create a new %1 Game").arg(game->name()));
	else
	{
		Player *master = game->master();
		item->setText( 0, i18n("Join %1's %2 Game").arg( (master ? master->name() : QString::null), game->name() ) );
		item->setText( 3, QString::number( game->players() ) );
		item->setEnabled( game->canBeJoined() );

		connect(master, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
	}
	m_gameList->triggerUpdate();

	validateConnectButton();
}

void SelectGame::playerChanged(Player *player)
{
	QListViewItem *item = m_gameList->firstChild();
	Game *game = 0;

	while (item)
	{
		game = m_atlanticCore->findGame( item->text(2).toInt() );
		if ( game && game->master() == player )
		{
			item->setText( 0, i18n("Join %1's %2 Game").arg( player->name(), game->name() ) );
			return;
		}
		item = item->nextSibling();
	}
}

QListViewItem *SelectGame::findItem(Game *game)
{
	QListViewItem *item = m_gameList->firstChild();
	while (item)
	{
		if ( (game->id() == -1 || item->text(2) == QString::number(game->id())) && item->text(4) == game->type() )
			return item;

		item = item->nextSibling();
	}
	return 0;
}

void SelectGame::validateConnectButton()
{
	if (QListViewItem *item = m_gameList->selectedItem())
	{
		if (item->text(2).toInt() > 0)
			m_connectButton->setText(i18n("Join Game"));
		else
			m_connectButton->setText(i18n("Create Game"));

		m_connectButton->setEnabled(true);
	}
	else
		m_connectButton->setEnabled(false);
}

void SelectGame::connectClicked()
{
	if (QListViewItem *item = m_gameList->selectedItem())
	{
		if (int gameId = item->text(2).toInt())
			emit joinGame(gameId);
		else
			emit newGame(item->text(4));
	}
}

#include "selectgame_widget.moc"
