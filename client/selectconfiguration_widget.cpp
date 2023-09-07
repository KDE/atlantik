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

#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>

#include <klocalizedstring.h>
#include <kguiitem.h>
#include <kstandardguiitem.h>

#include <atlantic_core.h>
#include <configoption.h>
#include <game.h>
#include <player.h>

#include "selectconfiguration_widget.h"

#include <atlantik_debug.h>

SelectConfiguration::SelectConfiguration(AtlanticCore *atlanticCore, QWidget *parent)
	: QWidget(parent)
	, m_game(nullptr)
	, m_atlanticCore(atlanticCore)
{
	m_mainLayout = new QVBoxLayout(this );
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
	Q_CHECK_PTR(m_mainLayout);

	const QPair<KGuiItem, KGuiItem> icons = KStandardGuiItem::backAndForward();

	// Game configuration.
	m_configBox = new QGroupBox(i18n("Game Configuration"), this);
	m_configBox->setObjectName(QStringLiteral("configBox"));
	m_mainLayout->addWidget(m_configBox);
	QVBoxLayout *configBoxLayout = new QVBoxLayout(m_configBox);
	Q_UNUSED(configBoxLayout)

	// Player buttons.
	QHBoxLayout *playerButtons = new QHBoxLayout();
        m_mainLayout->addItem( playerButtons );

	playerButtons->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	// Vertical spacer.
	m_mainLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

	// Server buttons.
	QHBoxLayout *serverButtons = new QHBoxLayout();
        m_mainLayout->addItem( serverButtons );

	m_backButton = new QPushButton(icons.first.icon(), i18n("Leave Game"), this);
	serverButtons->addWidget(m_backButton);

	connect(m_backButton, SIGNAL(clicked()), this, SIGNAL(leaveGame()));

	serverButtons->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_startButton = new QPushButton(icons.second.icon(), i18n("Start Game"), this);
	serverButtons->addWidget(m_startButton);
	m_startButton->setEnabled(false);

	connect(m_startButton, SIGNAL(clicked()), this, SIGNAL(startGame()));

	Player *playerSelf = m_atlanticCore->playerSelf();
	if (playerSelf) {
		playerChanged(playerSelf);
		connect(playerSelf, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
	}

	Q_EMIT statusMessage(i18n("Retrieving configuration list..."));

	if (m_game) {
		foreach (ConfigOption *opt, m_game->configOptions())
			addConfigOption(opt);

		connect(m_game, SIGNAL(createGUI(ConfigOption *)), this, SLOT(addConfigOption(ConfigOption *)));
	}

	slotEndUpdate();
}

void SelectConfiguration::initGame()
{
	Q_EMIT statusMessage(i18n("Game started. Retrieving full game data..."));
}

void SelectConfiguration::addConfigOption(ConfigOption *configOption)
{
	const QString type = configOption->type();
	if (type == QLatin1String("bool"))
		addConfigOptionBool(configOption);
	else
		qCDebug(ATLANTIK_LOG) << "unknown type" << type << "for option" << configOption->name();
}

void SelectConfiguration::addConfigOptionBool(ConfigOption *configOption)
{
	QCheckBox *checkBox = m_configBoxMap.value(configOption);

	if (!checkBox)
	{
		checkBox = new QCheckBox(configOption->description(), m_configBox);
		m_configBox->layout()->addWidget(checkBox);
		checkBox->setObjectName(QStringLiteral("checkbox"));
		m_configMap.insert((QObject *)checkBox, configOption);
		m_configBoxMap.insert(configOption, checkBox);

		connect(checkBox, SIGNAL(clicked()), this, SLOT(changeOption()));
		connect(configOption, SIGNAL(changed(ConfigOption *)), this, SLOT(optionChanged(ConfigOption *)));
	}

	checkBox->setChecked(configOption->value().toInt());
	checkBox->setEnabled(configOption->edit() && m_atlanticCore->selfIsMaster());
}

void SelectConfiguration::gameOption(const QString &title, const QString &type, const QString &value, const QString &edit, const QString &command)
{
	// Find if option exists in GUI yet
	if (QCheckBox *checkBox = dynamic_cast<QCheckBox *>(m_checkBoxMap[command]))
	{
		checkBox->setChecked(value.toInt());
		checkBox->setEnabled(edit.toInt());
		return;
	}

	// Create option
	if (type == QLatin1String("bool"))
	{
		QCheckBox *checkBox = new QCheckBox(title, m_configBox);
		m_configBox->layout()->addWidget(checkBox);
		checkBox->setObjectName(QStringLiteral("checkbox"));
		m_optionCommandMap[(QObject *)checkBox] = command;
		m_checkBoxMap[command] = checkBox;
		checkBox->setChecked(value.toInt());
		checkBox->setEnabled(edit.toInt());
		checkBox->show();

		connect(checkBox, SIGNAL(clicked()), this, SLOT(optionChanged()));
	}
	// TODO: create options other than type=bool

	// TODO: Enable edit for master only
}

void SelectConfiguration::changeOption()
{
	ConfigOption *configOption = m_configMap[(QObject *)QObject::sender()];
	if (configOption)
	{
		qCDebug(ATLANTIK_LOG) << "checked" << ((QCheckBox *)QObject::sender())->isChecked();
		Q_EMIT changeOption( configOption->id(), QString::number( ((QCheckBox *)QObject::sender())->isChecked() ) );
	}
}

void SelectConfiguration::optionChanged(ConfigOption *configOption)
{
	QCheckBox *checkBox = m_configBoxMap[configOption];
	if (checkBox)
	{
		checkBox->setText( configOption->description() );
		checkBox->setChecked( configOption->value().toInt() );
		checkBox->setEnabled( configOption->edit() && m_atlanticCore->selfIsMaster() );
	}
}

void SelectConfiguration::optionChanged()
{
	QString command = m_optionCommandMap[(QObject *)QObject::sender()];

	if (QCheckBox *checkBox = m_checkBoxMap[command])
	{
		command.append(QString::number(checkBox->isChecked()));
		Q_EMIT buttonCommand(command);
	}
}

void SelectConfiguration::slotEndUpdate()
{
	Q_EMIT statusMessage(i18n("Retrieved configuration list."));
}

void SelectConfiguration::playerChanged(Player *player)
{
	qCDebug(ATLANTIK_LOG);

	if (player->game() != m_game)
	{
		qCDebug(ATLANTIK_LOG) << "change";

		if (m_game)
		{
			disconnect(m_game, SIGNAL(createGUI(ConfigOption *)), this, SLOT(addConfigOption(ConfigOption *)));
			disconnect(m_game, SIGNAL(changed(Game *)), this, SLOT(gameChanged(Game *)));
		}

		m_game = player->game();

		if (m_game)
		{
			connect(m_game, SIGNAL(createGUI(ConfigOption *)), this, SLOT(addConfigOption(ConfigOption *)));
			connect(m_game, SIGNAL(changed(Game *)), this, SLOT(gameChanged(Game *)));
		}
	}
}

void SelectConfiguration::gameChanged(Game *game)
{
	m_startButton->setEnabled( game->master() == m_atlanticCore->playerSelf() );

QMapIterator<ConfigOption *, QCheckBox *> it(m_configBoxMap);
while (it.hasNext()) {
			it.next();
			it.value()->setEnabled(it.key()->edit() && m_atlanticCore->selfIsMaster() );

    }

}

#include "moc_selectconfiguration_widget.cpp"
