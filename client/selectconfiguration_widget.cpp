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
 
#include "selectconfiguration_widget.moc"

SelectConfiguration::SelectConfiguration(QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint());
	Q_CHECK_PTR(m_mainLayout);

	// Player list.
	m_playerBox = new QVGroupBox(i18n("Player List"), this, "playerBox");
	m_mainLayout->addWidget(m_playerBox); 

	// List of  players
	m_playerList = new KListView(m_playerBox, "m_playerList");
	m_playerList->addColumn(QString(i18n("Id")));
	m_playerList->addColumn(QString(i18n("Name")));
	m_playerList->addColumn(QString(i18n("Host")));
//	m_mainLayout->addWidget(m_playerList);

	connect(m_playerList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(connectPressed()));

	// Game configuration.
	m_configBox = new QVGroupBox(i18n("Game Configuration"), this, "configBox");
	m_mainLayout->addWidget(m_configBox); 

	QHBoxLayout *buttonBox = new QHBoxLayout(this, 0, KDialog::spacingHint());
	m_mainLayout->addItem(buttonBox);

	m_backButton = new KPushButton(SmallIcon("back"), i18n("Leave game"), this);
	buttonBox->addWidget(m_backButton);

	connect(m_backButton, SIGNAL(pressed()), this, SIGNAL(leaveGame()));

	buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_connectButton = new KPushButton(SmallIcon("forward"), i18n("Start Game"), this);
	buttonBox->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(pressed()), this, SLOT(connectPressed()));
	
    // Status indicator
	status_label = new QLabel(this);
	status_label->setText(i18n("Retrieving configuration list..."));
	m_mainLayout->addWidget(status_label);
}

void SelectConfiguration::slotPlayerListClear()
{
	m_playerList->clear();
//	emit statusChanged();
}

void SelectConfiguration::slotPlayerListAdd(QString playerId, QString name, QString host)
{
	kdDebug() << "SelectConfiguration::slotPlayerListAdd" << endl;
	QListViewItem *item = new QListViewItem(m_playerList, playerId, name, host);
	item->setPixmap(0, QPixmap(SmallIcon("personal")));
}

void SelectConfiguration::slotPlayerListEdit(QString playerId, QString name, QString host)
{
	QListViewItem *item = m_playerList->firstChild();
	while (item)
	{
		if (item->text(0) == playerId)
		{
			if (!name.isEmpty())
				item->setText(1, name);
			if (!host.isEmpty())
				item->setText(2, host);
			m_playerList->triggerUpdate();
			return;
		}
		item = item->nextSibling();
	}
//	emit statusChanged();
}

void SelectConfiguration::slotPlayerListDel(QString playerId)
{
	QListViewItem *item = m_playerList->firstChild();
	while (item)
	{
		if (item->text(0) == playerId)
		{
			delete item;
			return;
		}
		item = item->nextSibling();
	}
//	emit statusChanged();
}

void SelectConfiguration::connectPressed()
{
	emit startGame();
	return;

	if (QListViewItem *item = m_playerList->selectedItem())
	{
		if (int playerId = item->text(1).toInt())
			emit joinConfiguration(playerId);
		else
			emit newConfiguration();
	}

	m_messageBox = new QVGroupBox(i18n("Sorry"), this, "messageBox");
	m_mainLayout->addWidget(m_messageBox); 

	QLabel *label = new QLabel(m_messageBox);
	label->setText(i18n(
		"The new game wizard is undergoing a rewrite which has not been finished yet.\n"
		"You cannot start a game at the moment."
		));

	KPushButton *button = new KPushButton(i18n("OK"), m_messageBox, "button");

	m_playerBox->setEnabled(false);
	m_configBox->setEnabled(false);
	m_connectButton->setEnabled(false);
	status_label->setEnabled(false);

	connect(button, SIGNAL(clicked()), this, SLOT(slotClicked()));

	m_messageBox->show();
}

void SelectConfiguration::slotClicked()
{
	delete m_messageBox;
	m_playerBox->setEnabled(true);
	m_configBox->setEnabled(true);
	m_connectButton->setEnabled(true);
	status_label->setEnabled(true);
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
	else
		kdDebug() << "TODO: game options other than type=bool" << endl;

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
