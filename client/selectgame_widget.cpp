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
#include <QPushButton>

#include <klocalizedstring.h>
#include <kiconloader.h>
#include <knotification.h>
#include <kguiitem.h>
#include <kstandardguiitem.h>

#include <atlantic_core.h>
#include <game.h>
#include <player.h>

#include "selectgame_widget.h"

enum { GameTypeRole = Qt::UserRole + 1 };

SelectGame::SelectGame(AtlanticCore *atlanticCore, QWidget *parent) 
	: QWidget(parent)
	, m_atlanticCore(atlanticCore)
{
	connect(m_atlanticCore, SIGNAL(createGUI(Game *)), this, SLOT(addGame(Game *)));
	connect(m_atlanticCore, SIGNAL(removeGUI(Game *)), this, SLOT(delGame(Game *)));

	setupUi(this);
	layout()->setContentsMargins(0, 0, 0, 0);

	const QPair<KGuiItem, KGuiItem> icons = KStandardGuiItem::backAndForward();

	// List of games
	m_gameList->header()->setSectionsClickable(false);

	connect(m_gameList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(validateConnectButton()));
	connect(m_gameList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(connectClicked()));
	connect(m_gameList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(validateConnectButton()));

	backButton->setIcon(icons.first.icon());

	connect(backButton, SIGNAL(clicked()), this, SIGNAL(leaveServer()));

	m_connectButton->setIcon(icons.second.icon());
	m_connectButton->setEnabled(false);

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
		item->setIcon(0, KDE::icon(QStringLiteral("document-new")));
		m_gameList->addTopLevelItem(item);
	}
	else
	{
		Player *master = game->master();
		QTreeWidgetItem *item = new QTreeWidgetItem();
		if (!game->canBeJoined() && game->canBeWatched())
			item->setText(0, i18n("Watch %1's %2 Game", (master ? master->name() : QString()), game->name()));
		else
			item->setText(0, i18n("Join %1's %2 Game", (master ? master->name() : QString()), game->name()));
		item->setText(1, game->description());
		item->setText(2, QString::number(game->id()));
		item->setText(3, QString::number(game->players()));
		item->setData(0, GameTypeRole, game->type());
		item->setIcon(0, KDE::icon(QStringLiteral("atlantik")));
		item->setDisabled(!game->canBeJoined() && !game->canBeWatched());
		m_gameList->addTopLevelItem(item);

		KNotification::event(QStringLiteral("newgame"), i18n("New game available."));

		if (master)
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
		if (!game->canBeJoined() && game->canBeWatched())
			item->setText(0, i18n("Watch %1's %2 Game", (master ? master->name() : QString()), game->name()));
		else
			item->setText(0, i18n("Join %1's %2 Game", (master ? master->name() : QString()), game->name()));
		item->setText( 3, QString::number( game->players() ) );
		item->setDisabled(!game->canBeJoined() && !game->canBeWatched());

		connect(master, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
	}

	validateConnectButton();
}

void SelectGame::playerChanged(Player *player)
{
	const int count = m_gameList->topLevelItemCount();
	Game *game = nullptr;

	for (int i = 0; i < count; ++i)
	{
		QTreeWidgetItem *item = m_gameList->topLevelItem(i);
		game = m_atlanticCore->findGame( item->text(2).toInt() );
		if ( game && game->master() == player )
		{
			if (!game->canBeJoined() && game->canBeWatched())
				item->setText( 0, i18n("Watch %1's %2 Game", player->name(), game->name() ) );
			else
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
	return nullptr;
}

void SelectGame::validateConnectButton()
{
	const QList<QTreeWidgetItem *> items = m_gameList->selectedItems();
	if (!items.isEmpty())
	{
		const QTreeWidgetItem *item = items.first();
		if (item->text(2).toInt() > 0)
		{
			Game *game = m_atlanticCore->findGame(item->text(2).toInt());
			if (!game->canBeJoined() && game->canBeWatched())
				m_connectButton->setText(i18n("Watch Game"));
			else
				m_connectButton->setText(i18n("Join Game"));
		}
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
		{
			Game *game = m_atlanticCore->findGame(item->text(2).toInt());
			if (!game->canBeJoined() && game->canBeWatched())
				Q_EMIT watchGame(gameId);
			else
				Q_EMIT joinGame(gameId);
		}
		else
			Q_EMIT newGame(item->data(0, GameTypeRole).toString());
	}
}

#include "moc_selectgame_widget.cpp"
