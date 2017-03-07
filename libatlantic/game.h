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

#ifndef LIBATLANTIC_GAME_H
#define LIBATLANTIC_GAME_H

#include <QObject>

#include "libatlantic_export.h"

class Player;
class ConfigOption;

class LIBATLANTIC_EXPORT Game : public QObject
{
Q_OBJECT

public:
	Game(int gameId);
	~Game();

	int id() const;
	void setCanBeJoined(bool canBeJoined);
	bool canBeJoined() const;
	void setDescription(const QString &description);
	QString description() const;
	void setName(const QString &name);
	QString name() const;
	void setType(const QString &type);
	QString type() const;
	int players() const;
	void setPlayers(int players);
	Player *master() const;
	void setMaster(Player *master);
	void setCanBeWatched(bool canBeWatched);
	bool canBeWatched() const;

	QList<ConfigOption *> configOptions() const;
	void addConfigOption(ConfigOption *configOption);
	void removeConfigOption(ConfigOption *configOption);
	ConfigOption *findConfigOption(int configId) const;
	ConfigOption *findConfigOption(const QString &name) const;

	void update(bool force = false);

Q_SIGNALS:
	void changed(Game *game);
	void createGUI(ConfigOption *configOption);
	void removeGUI(ConfigOption *configOption);

private:
	bool m_changed;
	bool m_canBeJoined;
	bool m_canBeWatched;
	QString m_description, m_name, m_type;
	int m_id, m_players;
	Player *m_master;
	QList<ConfigOption *> m_configOptions;
};

#endif
