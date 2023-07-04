// Copyright (c) 2003 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.



#include "game.h"

#include "configoption.h"

Game::Game(int gameId)
	: QObject()
	, m_changed(false)
	, m_canBeJoined(false)
	, m_canBeWatched(false)
	, m_id(gameId)
	, m_players(0)
	, m_master(nullptr)
{
}

Game::~Game()
{
	qDeleteAll(m_configOptions);
}

int Game::id() const
{
	return m_id;
}

void Game::setCanBeJoined(bool canBeJoined)
{
	if (m_canBeJoined != canBeJoined)
	{
		m_canBeJoined = canBeJoined;
		m_changed = true;
	}
}

bool Game::canBeJoined() const
{
	return m_canBeJoined;
}

void Game::setDescription(const QString &description)
{
	if (m_description != description)
	{
		m_description = description;
		m_changed = true;
	}
}

QString Game::description() const
{
	return m_description;
}

void Game::setName(const QString &name)
{
	if (m_name != name)
	{
		m_name = name;
		m_changed = true;
	}
}

QString Game::name() const
{
	return m_name;
}

void Game::setType(const QString &type)
{
	if (m_type != type)
	{
		m_type = type;
		m_changed = true;
	}
}

QString Game::type() const
{
	return m_type;
}

void Game::update(bool force)
{
	if (m_changed || force)
	{
		Q_EMIT changed(this);
		m_changed = false;
	}
}

int Game::players() const
{
	return m_players;
}

void Game::setPlayers(int players)
{
	if (m_players != players)
	{
		m_players = players;
		m_changed = true;
	}
}

Player *Game::master() const
{
	return m_master;
}

void Game::setMaster(Player *master)
{
	if (m_master != master)
	{
		m_master = master;
		m_changed = true;
	}
}

void Game::setCanBeWatched(bool canBeWatched)
{
	if (m_canBeWatched != canBeWatched)
	{
		m_canBeWatched = canBeWatched;
		m_changed = true;
	}
}

bool Game::canBeWatched() const
{
	return m_canBeWatched;
}

QList<ConfigOption *> Game::configOptions() const
{
	return m_configOptions;
}

void Game::addConfigOption(ConfigOption *configOption)
{
	m_configOptions.append(configOption);
	Q_EMIT createGUI(configOption);
}

void Game::removeConfigOption(ConfigOption *configOption)
{
	m_configOptions.removeOne(configOption);
	Q_EMIT removeGUI(configOption);
	configOption->deleteLater();
}

ConfigOption *Game::findConfigOption(int configId) const
{
	foreach (ConfigOption *configOption, m_configOptions)
		if (configOption->id() == configId)
			return configOption;

	return nullptr;
}

ConfigOption *Game::findConfigOption(const QString &name) const
{
	foreach (ConfigOption *configOption, m_configOptions)
		if (configOption->name() == name)
			return configOption;

	return nullptr;
}

#include "moc_game.cpp"
