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

#include <iostream>

#include <qcheckbox.h>
#include <qradiobutton.h>

#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#include <atlantic_core.h>
#include <configoption.h>
#include <game.h>
#include <player.h>

#include "selectconfiguration_widget.moc"

SelectConfiguration::SelectConfiguration(AtlanticCore *atlanticCore, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_atlanticCore = atlanticCore;
	m_game = 0;

	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint());
	Q_CHECK_PTR(m_mainLayout);

	// Game configuration.
	m_configBox = new QVGroupBox(i18n("Game Configuration"), this, "configBox");
	m_mainLayout->addWidget(m_configBox);

	// Player buttons.
	QHBoxLayout *playerButtons = new QHBoxLayout(m_mainLayout, KDialog::spacingHint());
	playerButtons->setMargin(0);

	playerButtons->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	// Vertical spacer.
	m_mainLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

	// Server buttons.
	QHBoxLayout *serverButtons = new QHBoxLayout(m_mainLayout, KDialog::spacingHint());
	serverButtons->setMargin(0);

	m_backButton = new KPushButton(SmallIcon("back"), i18n("Leave Game"), this);
	serverButtons->addWidget(m_backButton);

	connect(m_backButton, SIGNAL(clicked()), this, SIGNAL(leaveGame()));

	serverButtons->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_startButton = new KPushButton(SmallIconSet("forward"), i18n("Start Game"), this);
	serverButtons->addWidget(m_startButton);
	m_startButton->setEnabled(false);

	connect(m_startButton, SIGNAL(clicked()), this, SIGNAL(startGame()));

	Player *playerSelf = m_atlanticCore->playerSelf();
	playerChanged(playerSelf);
	connect(playerSelf, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
	
	emit statusMessage(i18n("Retrieving configuration list..."));
}

void SelectConfiguration::initGame()
{
	emit statusMessage(i18n("Game started. Retrieving full game data..."));
}

void SelectConfiguration::addConfigOption(ConfigOption *configOption)
{
	// FIXME: only bool types supported!
	QCheckBox *checkBox = new QCheckBox(configOption->description(), m_configBox, "checkbox");
	m_configMap[(QObject *)checkBox] = configOption;
	m_configBoxMap[configOption] = checkBox;
	
	checkBox->setChecked( configOption->value().toInt() );
	checkBox->setEnabled( configOption->edit() && m_atlanticCore->selfIsMaster() );
	checkBox->show();

	connect(checkBox, SIGNAL(clicked()), this, SLOT(changeOption()));
	connect(configOption, SIGNAL(changed(ConfigOption *)), this, SLOT(optionChanged(ConfigOption *)));
}

void SelectConfiguration::gameOption(QString title, QString type, QString value, QString edit, QString command)
{
	// Find if option exists in GUI yet
	if (QCheckBox *checkBox = dynamic_cast<QCheckBox *>(m_checkBoxMap[command]))
	{
		checkBox->setChecked(value.toInt());
		checkBox->setEnabled(edit.toInt());
		return;
	}

	// Create option
	if (type == "bool")
	{
		QCheckBox *checkBox = new QCheckBox(title, m_configBox, "checkbox");
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
		kdDebug() << "checked " << ((QCheckBox *)QObject::sender())->isChecked() << endl;
		emit changeOption( configOption->id(), QString::number( ((QCheckBox *)QObject::sender())->isChecked() ) );
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
		emit buttonCommand(command);
	}
}

void SelectConfiguration::slotEndUpdate()
{
	emit statusMessage(i18n("Retrieved configuration list."));
}

void SelectConfiguration::playerChanged(Player *player)
{
	kdDebug() << "playerChanged" << endl;

	if (player->game() != m_game)
	{
		kdDebug() << "playerChanged::change" << endl;

		if (m_game)
			disconnect(m_game, SIGNAL(changed(Game *)), this, SLOT(gameChanged(Game *)));

		m_game = player->game();

		if (m_game)
			connect(m_game, SIGNAL(changed(Game *)), this, SLOT(gameChanged(Game *)));
	}
}

void SelectConfiguration::gameChanged(Game *game)
{
	m_startButton->setEnabled( game->master() == m_atlanticCore->playerSelf() );

	for (QMapIterator<ConfigOption *, QCheckBox *> it = m_configBoxMap.begin() ; it != m_configBoxMap.end() ; ++it)
		(*it)->setEnabled( it.key()->edit() && m_atlanticCore->selfIsMaster() );
}
