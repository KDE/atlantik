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

#include <iostream>

#include <qcheckbox.h>
#include <qradiobutton.h>

#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#include <player.h>

#include "tokenwidget.h" 
#include "selectconfiguration_widget.moc"

SelectConfiguration::SelectConfiguration(QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_tokenWidget = 0;

	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint());
	Q_CHECK_PTR(m_mainLayout);

	// Game configuration.
	m_configBox = new QVGroupBox(i18n("Game Configuration"), this, "configBox");
	m_mainLayout->addWidget(m_configBox); 

	// Player buttons.
	QHBoxLayout *playerButtons = new QHBoxLayout(this, 0, KDialog::spacingHint());
	m_mainLayout->addItem(playerButtons);

	playerButtons->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_tokenButton = new KPushButton(SmallIcon("personal"), i18n("Select token"), this);
	playerButtons->addWidget(m_tokenButton);

	connect(m_tokenButton, SIGNAL(clicked()), this, SLOT(slotTokenButtonClicked()));

	// Vertical spacer.
	m_mainLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

	// Server buttons.
	QHBoxLayout *serverButtons = new QHBoxLayout(this, 0, KDialog::spacingHint());
	m_mainLayout->addItem(serverButtons);

	m_backButton = new KPushButton(SmallIcon("back"), i18n("Leave Game"), this);
	serverButtons->addWidget(m_backButton);

	connect(m_backButton, SIGNAL(clicked()), this, SIGNAL(leaveGame()));

	serverButtons->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_connectButton = new KPushButton(SmallIcon("forward"), i18n("Start Game"), this);
	serverButtons->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));

    // Status indicator.
	status_label = new QLabel(this);
	status_label->setText(i18n("Retrieving configuration list..."));
	m_mainLayout->addWidget(status_label);
}

void SelectConfiguration::connectClicked()
{
	status_label->setText(i18n("Game started. Retrieving full game data..."));
	emit startGame();
}

void SelectConfiguration::slotTokenButtonClicked()
{
	if (m_tokenWidget)
		return;

	m_tokenWidget = new TokenWidget(0);
	m_tokenWidget->show();

	connect(m_tokenWidget, SIGNAL(iconSelected(const QString &)), this, SLOT(slotTokenSelected(const QString &)));
}

void SelectConfiguration::slotTokenSelected(const QString &name)
{
	emit iconSelected(name);
	m_tokenWidget->close();
//	m_tokenWidget->deleteLater(); // FIXME: crashes?
	m_tokenWidget = 0;
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

/*
// some old code i might want to reuse

void ConfigureGame::slotPlayerlistEndUpdate(QString type)
{
	if (type=="full")
		status_label->setText(i18n("Fetched list of players."));

	emit statusChanged();
}

bool ConfigureGame::validateNext()
{
	if (list->childCount() >= 2)
		return true;
	else
		return false;
}
*/

void SelectConfiguration::optionChanged()
{
	QString command = m_optionCommandMap[(QObject *)QObject::sender()];

	if (QCheckBox *checkBox = m_checkBoxMap[command])
	{
		command.append(QString::number(checkBox->isChecked()));
		emit buttonCommand(command);
	}
}
