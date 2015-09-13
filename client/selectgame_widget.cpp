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

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>

#include <kdebug.h>
#include <klocalizedstring.h>
#include <kicon.h>
#include <knotification.h>
#include <kpushbutton.h>

#include <atlantic_core.h>
#include <game.h>
#include <player.h>

#include "selectgame_widget.h"

enum { GameTypeRole = Qt::UserRole + 1 };

SelectGame::SelectGame(AtlanticCore *atlanticCore, QWidget *parent) 
        : QWidget(parent)
{
	m_atlanticCore = atlanticCore;

	connect(m_atlanticCore, SIGNAL(createGUI(Game *)), this, SLOT(addGame(Game *)));
	connect(m_atlanticCore, SIGNAL(removeGUI(Game *)), this, SLOT(delGame(Game *)));

	m_mainLayout = new QVBoxLayout( this );
        Q_CHECK_PTR(m_mainLayout);
        m_mainLayout->setMargin(0);

	QGroupBox *groupBox = new QGroupBox(i18n("Create or Select monopd Game"), this);
	groupBox->setObjectName(QLatin1String("groupBox"));
	m_mainLayout->addWidget(groupBox);
	QVBoxLayout *groupBoxLayout = new QVBoxLayout(groupBox);

	// List of games
	m_gameList = new QTreeWidget(groupBox);
	groupBoxLayout->addWidget(m_gameList);
	m_gameList->setObjectName( "m_gameList" );
	QStringList headers;
	headers << i18n("Game");
	headers << i18n("Description");
	headers << i18n("Id");
	headers << i18n("Players");
	m_gameList->setHeaderLabels(headers);
	m_gameList->setRootIsDecorated(false);
	m_gameList->setAllColumnsShowFocus(true);
	m_gameList->header()->setClickable(false);
//	m_mainLayout->addWidget(m_gameList);

	connect(m_gameList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(validateConnectButton()));
	connect(m_gameList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(connectClicked()));
	connect(m_gameList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(validateConnectButton()));

	QHBoxLayout *buttonBox = new QHBoxLayout();
        m_mainLayout->addItem( buttonBox );

	KPushButton *backButton = new KPushButton(KIcon("go-previous"), i18n("Server List"), this);
	buttonBox->addWidget(backButton);

	connect(backButton, SIGNAL(clicked()), this, SIGNAL(leaveServer()));

	buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_connectButton = new KPushButton(KIcon("go-next"), i18n("Create Game"), this);
	m_connectButton->setEnabled(false);
	buttonBox->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));

}

void SelectGame::addGame(Game *game)
{
	connect(game, SIGNAL(changed(Game *)), this, SLOT(updateGame(Game *)));

	if (game->id() == -1)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, i18n("Create a new %1 Game", game->name()));
		item->setText(1, game->description());
		item->setData(0, GameTypeRole, game->type());
		item->setIcon(0, KIcon("document-new"));
		m_gameList->addTopLevelItem(item);
	}
	else
	{
		Player *master = game->master();
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, i18n("Join %1's %2 Game", (master ? master->name() : QString()), game->name()));
		item->setText(1, game->description());
		item->setText(2, QString::number(game->id()));
		item->setText(3, QString::number(game->players()));
		item->setData(0, GameTypeRole, game->type());
		item->setIcon(0, KIcon("atlantik"));
		item->setDisabled(!game->canBeJoined());
		m_gameList->addTopLevelItem(item);

		KNotification::event("newgame");

		connect(master, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
	}
	m_gameList->resizeColumnToContents(0);
	m_gameList->resizeColumnToContents(1);
	m_gameList->resizeColumnToContents(2);
	m_gameList->resizeColumnToContents(3);

//	validateConnectButton();
}

void SelectGame::delGame(Game *game)
{
	QTreeWidgetItem *item = findItem(game);
	if (!item)
		return;

	delete item;

	validateConnectButton();
}

void SelectGame::updateGame(Game *game)
{
	QTreeWidgetItem *item = findItem(game);
	if (!item)
		return;

	item->setText( 1, game->description() );

	if (game->id() == -1)
		item->setText(0, i18n("Create a new %1 Game", game->name()));
	else
	{
		Player *master = game->master();
		item->setText( 0, i18n("Join %1's %2 Game", (master ? master->name() : QString::null), game->name() ) );
		item->setText( 3, QString::number( game->players() ) );
		item->setDisabled(!game->canBeJoined());

		connect(master, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
	}

	validateConnectButton();
}

void SelectGame::playerChanged(Player *player)
{
	const int count = m_gameList->topLevelItemCount();
	Game *game = 0;

	for (int i = 0; i < count; ++i)
	{
		QTreeWidgetItem *item = m_gameList->topLevelItem(i);
		game = m_atlanticCore->findGame( item->text(2).toInt() );
		if ( game && game->master() == player )
		{
			item->setText( 0, i18n("Join %1's %2 Game", player->name(), game->name() ) );
			return;
		}
	}
}

QTreeWidgetItem *SelectGame::findItem(Game *game)
{
	const int count = m_gameList->topLevelItemCount();
	for (int i = 0; i < count; ++i)
	{
		QTreeWidgetItem *item = m_gameList->topLevelItem(i);
		if ( (game->id() == -1 || item->text(2) == QString::number(game->id())) && item->data(0, GameTypeRole).toString() == game->type() )
			return item;
	}
	return 0;
}

void SelectGame::validateConnectButton()
{
	const QList<QTreeWidgetItem *> items = m_gameList->selectedItems();
	if (!items.isEmpty())
	{
		const QTreeWidgetItem *item = items.first();
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
	const QList<QTreeWidgetItem *> items = m_gameList->selectedItems();
	if (!items.isEmpty())
	{
		const QTreeWidgetItem *item = items.first();
		if (int gameId = item->text(2).toInt())
			emit joinGame(gameId);
		else
			emit newGame(item->data(0, GameTypeRole).toString());
	}
}

#include "selectgame_widget.moc"
