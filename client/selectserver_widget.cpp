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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <qlayout.h>
#include <qradiobutton.h>
#include <qsizepolicy.h>
#include <qvbuttongroup.h>
#include <qhgroupbox.h>

#include <kdialog.h>
#include <kextendedsocket.h>
#include <klocale.h>
#include <kiconloader.h>

#include "selectserver_widget.moc"

SelectServer::SelectServer(bool useMonopigatorOnStart, bool hideDevelopmentServers, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_hideDevelopmentServers = hideDevelopmentServers;

	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint());
	Q_CHECK_PTR(m_mainLayout);

	// Custom server group
	QHGroupBox *customGroup = new QHGroupBox(i18n("Enter custom monopd Server"), this, "customGroup");
	m_mainLayout->addWidget(customGroup);

	QLabel *hostLabel = new QLabel(i18n("Hostname"), customGroup);

	m_hostEdit = new KLineEdit(customGroup);
	m_hostEdit->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

	QLabel *portLabel = new QLabel(i18n("Port"), customGroup);

	m_portEdit = new KLineEdit(QString::number(1234), customGroup);
	m_portEdit->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum));

	KPushButton *connectButton = new KPushButton( KGuiItem(i18n("Connect"), "network"), customGroup);
	connect(connectButton, SIGNAL(clicked()), this, SLOT(customConnect()));

	// Server list group
	QVButtonGroup *bgroup = new QVButtonGroup(i18n("Select monopd Server"), this, "bgroup");
	bgroup->setExclusive(true);
	m_mainLayout->addWidget(bgroup);

	// List of servers
	m_serverList = new KListView(bgroup, "m_serverList");
	m_serverList->addColumn(i18n("Host"));
	m_serverList->addColumn(i18n("Latency"));
	m_serverList->addColumn(i18n("Version"));
	m_serverList->addColumn(i18n("Users"));
	m_serverList->setAllColumnsShowFocus(true);
	m_serverList->setSorting(1);
//	m_mainLayout->addWidget(m_serverList);

	connect(m_serverList, SIGNAL(clicked(QListViewItem *)), this, SLOT(validateConnectButton()));
	connect(m_serverList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(slotConnect()));
	connect(m_serverList, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)), this, SLOT(validateConnectButton()));
	connect(m_serverList, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(validateConnectButton()));

	QHBoxLayout *buttonBox = new QHBoxLayout(m_mainLayout, KDialog::spacingHint());
	buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	// Server List / Refresh
	m_refreshButton = new KPushButton( KGuiItem(useMonopigatorOnStart ? i18n("Reload Server List") : i18n("Get Server List"), useMonopigatorOnStart ? "reload" : "network"), this);
	buttonBox->addWidget(m_refreshButton);

	connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(slotRefresh()));

	// Connect
	m_connectButton = new KPushButton(BarIcon("forward", KIcon::SizeSmall), i18n("Connect"), this);
	m_connectButton->setEnabled(false);
	buttonBox->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(clicked()), this, SLOT(slotConnect()));

//	Status indicator
	status_label = new QLabel(this);
	m_mainLayout->addWidget(status_label);

	// Monopigator
	m_monopigator = new Monopigator();

	connect(m_monopigator, SIGNAL(monopigatorAdd(QString, QString, QString, int)), this, SLOT(slotMonopigatorAdd(QString, QString, QString, int)));
	connect(m_monopigator, SIGNAL(finished()), SLOT(monopigatorFinished()));
	connect(m_monopigator, SIGNAL(timeout()), SLOT(monopigatorTimeout()));

	slotRefresh(useMonopigatorOnStart);
}

SelectServer::~SelectServer()
{
	delete m_monopigator;
}

void SelectServer::setHideDevelopmentServers(bool hideDevelopmentServers)
{
	m_hideDevelopmentServers = hideDevelopmentServers;
}

void SelectServer::initMonopigator()
{
	// Hardcoded, but there aren't any other Monopigator root servers at the moment
	status_label->setText(i18n("Retrieving server list..."));
	m_refreshButton->setGuiItem(KGuiItem(i18n("Reload Server List"), "reload"));
	m_monopigator->loadData("http://gator.monopd.net/");
}

void SelectServer::slotMonopigatorAdd(QString host, QString port, QString version, int users)
{
	if (m_hideDevelopmentServers && version.contains("CVS"))
		return;

	MonopigatorEntry *item = new MonopigatorEntry(m_serverList, host, QString::number(9999), version, (users == -1) ? i18n("unknown") : QString::number(users), port);
	item->setPixmap(0, BarIcon("atlantik", KIcon::SizeSmall));
	validateConnectButton();
}

void SelectServer::monopigatorFinished()
{
	status_label->setText(i18n("Retrieved server list."));
	m_refreshButton->setEnabled(true);
}

void SelectServer::monopigatorTimeout()
{
	status_label->setText(i18n("Error while retrieving the server list."));
	m_refreshButton->setEnabled(true);
}

void SelectServer::validateConnectButton()
{
	if (m_serverList->selectedItem())
		m_connectButton->setEnabled(true);
	else
		m_connectButton->setEnabled(false);
}

void SelectServer::slotRefresh(bool useMonopigator)
{
	m_serverList->clear();
	validateConnectButton();

	if (useMonopigator)
	{
		m_refreshButton->setEnabled(false);
		initMonopigator();
	}
}

void SelectServer::slotConnect()
{
	if (QListViewItem *item = m_serverList->selectedItem())
		emit serverConnect(item->text(0), item->text(4).toInt());
}

void SelectServer::customConnect()
{
	if (!m_hostEdit->text().isEmpty() && !m_portEdit->text().isEmpty())
		emit serverConnect(m_hostEdit->text(), m_portEdit->text().toInt());
}
