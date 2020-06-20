// Copyright (c) 2002-2004 Rob Kaper <cap@capsi.com>
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

#ifndef ATLANTIK_SELECTCONFIGURATION_WIDGET_H
#define ATLANTIK_SELECTCONFIGURATION_WIDGET_H

#include <QMap>
#include <QWidget>

class QCheckBox;
class QGroupBox;
class QPushButton;
class QVBoxLayout;

class AtlanticCore;
class ConfigOption;
class Game;
class Player;

class SelectConfiguration : public QWidget
{
Q_OBJECT

public:
	SelectConfiguration(AtlanticCore *atlanticCore, QWidget *parent);

private Q_SLOTS:
	void addConfigOption(ConfigOption *configOption);
	void changeOption();
	void gameOption(const QString &title, const QString &type, const QString &value, const QString &edit, const QString &command);
	void optionChanged(ConfigOption *configOption);
	void optionChanged();
	void slotEndUpdate();
	void initGame();
	void playerChanged(Player *player);
	void gameChanged(Game *game);

Q_SIGNALS:
	void startGame();
	void leaveGame();
	void joinConfiguration(int configurationId);
	void newConfiguration();
	void changeOption(int configId, const QString &value);
	void buttonCommand(const QString &);
	void iconSelected(const QString &);
	void statusMessage(const QString &message);

private:
	void addConfigOptionBool(ConfigOption *configOption);

	QVBoxLayout *m_mainLayout;
	QGroupBox *m_configBox, *m_messageBox;
	QPushButton *m_backButton, *m_startButton;
	QMap <QObject *, QString> m_optionCommandMap;
	QMap <QObject *, ConfigOption *> m_configMap;
	QMap <ConfigOption *, QCheckBox *> m_configBoxMap;
	QMap <QString, QCheckBox *> m_checkBoxMap;
	Game *m_game;
	AtlanticCore *m_atlanticCore;
};

#endif
