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

#include <qvbuttongroup.h>
#include <qradiobutton.h>

#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>

#include "selectserver_widget.moc"

SelectServer::SelectServer(QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint());
	Q_CHECK_PTR(m_mainLayout);

	QVButtonGroup *bgroup;
//	bgroup = new QVButtonGroup(i18n("Start or select a monopd server"), this, "bgroup");
	bgroup = new QVButtonGroup(i18n("Select a monopd Server"), this, "bgroup");
	bgroup->setExclusive(true);
	m_mainLayout->addWidget(bgroup); 

	// Button for local games
//	m_localGameButton = new QRadioButton(QString(i18n("Start a local server")), bgroup, "m_localGameButton");
//	connect(m_localGameButton, SIGNAL(stateChanged(int)), this, SLOT(validateConnectButton())); 

	// Button for on-line games
//	m_onlineGameButton = new QRadioButton(QString(i18n("Select a server to play a game on-line")), bgroup, "m_onlineGameButton");
//	m_localGameButton->setEnabled(true);
//	connect(m_onlineGameButton, SIGNAL(stateChanged(int)), this, SLOT(validateConnectButton()));

	// List of servers
	m_serverList = new KListView(bgroup, "m_serverList");
	m_serverList->addColumn(QString(i18n("Host")));
	m_serverList->addColumn(QString(i18n("Version")));
	m_serverList->addColumn(QString(i18n("Users")));
//	m_mainLayout->addWidget(m_serverList);

	connect(m_serverList, SIGNAL(clicked(QListViewItem *)), this, SLOT(validateConnectButton()));
//	connect(m_serverList, SIGNAL(clicked(QListViewItem *)), this, SLOT(slotListClicked(QListViewItem *)));
	connect(m_serverList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(connectPressed()));
	connect(m_serverList, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)), this, SLOT(validateConnectButton()));
	connect(m_serverList, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(validateConnectButton()));

	QHBoxLayout *buttonBox = new QHBoxLayout(this, 0, KDialog::spacingHint());
	m_mainLayout->addItem(buttonBox); 

	buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_refreshButton = new KPushButton(BarIcon("reload", KIcon::SizeSmall), i18n("Refresh"), this);
	buttonBox->addWidget(m_refreshButton);

	connect(m_refreshButton, SIGNAL(pressed()), this, SLOT(initMonopigator()));

	m_connectButton = new KPushButton(BarIcon("forward", KIcon::SizeSmall), i18n("Connect"), this);
	m_connectButton->setEnabled(false);
	buttonBox->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(pressed()), this, SLOT(connectPressed()));
	
    // Status indicator
	status_label = new QLabel(this);
	m_mainLayout->addWidget(status_label);

	// Monopigator
	monopigator = new Monopigator();

	connect(monopigator, SIGNAL(monopigatorClear()), this, SLOT(slotMonopigatorClear()));
	connect(monopigator, SIGNAL(monopigatorAdd(QString, QString, QString, int)), this, SLOT(slotMonopigatorAdd(QString, QString, QString, int)));
	connect(monopigator, SIGNAL(finished()), SLOT(monopigatorFinished()));

	// Until we have a good way to use start a local monopd server, disable this button
//	m_localGameButton->setEnabled(false);

	initMonopigator();
}

void SelectServer::initMonopigator()
{
	// Hardcoded, but there aren't any other Monopigator root servers at the moment
	status_label->setText(i18n("Retrieving server list..."));
	monopigator->loadData("http://gator.monopd.net/");
}

void SelectServer::slotMonopigatorClear()
{
	m_serverList->clear();
	validateConnectButton();
//	emit statusChanged();
}

void SelectServer::slotMonopigatorAdd(QString host, QString port, QString version, int users)
{
	QListViewItem *item = new QListViewItem(m_serverList, host, version, (users == -1) ? i18n("unknown") : QString::number(users), port);
	item->setPixmap(0, BarIcon("atlantik", KIcon::SizeSmall));
	validateConnectButton();
}

void SelectServer::monopigatorFinished()
{
	// TODO : remove, this is temporarily until there is a good way to fork a server
	QListViewItem *item = new QListViewItem(m_serverList, "localhost", i18n("unknown"), i18n("unknown"), QString::number(1234));
	item->setPixmap(0, BarIcon("atlantik", KIcon::SizeSmall));
	validateConnectButton();
	status_label->setText(i18n("Retrieved server list."));
}

void SelectServer::validateRadioButtons()
{
	return;
	if (m_serverList->childCount() > 0)
	{
		if (!m_onlineGameButton->isEnabled())
			m_onlineGameButton->setEnabled(true);
	}
	else
	{
		if (m_onlineGameButton->isEnabled())
			m_onlineGameButton->setEnabled(false);
		m_serverList->clearSelection();
	}
}

void SelectServer::validateConnectButton()
{
	validateRadioButtons();

//	if ( (m_localGameButton->isEnabled() && m_localGameButton->isChecked()) || (m_onlineGameButton->isEnabled() && m_onlineGameButton->isChecked() && m_serverList->selectedItem()) )
	if (m_serverList->selectedItem())
		m_connectButton->setEnabled(true);
	else
		m_connectButton->setEnabled(false);
}

void SelectServer::slotListClicked(QListViewItem *item)
{
	// Simulate a user press
	if(item && !m_onlineGameButton->isOn())
		m_onlineGameButton->toggle();
}

void SelectServer::connectPressed()
{
	if (QListViewItem *item = m_serverList->selectedItem())
		emit serverConnect(item->text(0), item->text(3).toInt());
}
