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

#include <qvbuttongroup.h>
#include <qradiobutton.h>

#include <kdialog.h>
#include <kextendedsocket.h>
#include <klineeditdlg.h>
#include <klocale.h>
#include <kiconloader.h>

#include "selectserver_widget.moc"

SelectServer::SelectServer(bool useMonopigatorOnStart, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint());
	Q_CHECK_PTR(m_mainLayout);

	QVButtonGroup *bgroup;
	bgroup = new QVButtonGroup(i18n("Select monopd Server"), this, "bgroup");
	bgroup->setExclusive(true);
	m_mainLayout->addWidget(bgroup);

	// List of servers
	m_serverList = new KListView(bgroup, "m_serverList");
	m_serverList->addColumn(i18n("Host"));
	m_serverList->addColumn(i18n("Version"));
	m_serverList->addColumn(i18n("Users"));
	m_serverList->setAllColumnsShowFocus(true);
//	m_mainLayout->addWidget(m_serverList);

	connect(m_serverList, SIGNAL(clicked(QListViewItem *)), this, SLOT(validateConnectButton()));
//	connect(m_serverList, SIGNAL(clicked(QListViewItem *)), this, SLOT(slotListClicked(QListViewItem *)));
	connect(m_serverList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(slotConnect()));
	connect(m_serverList, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)), this, SLOT(validateConnectButton()));
	connect(m_serverList, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(validateConnectButton()));

	QHBoxLayout *buttonBox = new QHBoxLayout(m_mainLayout, KDialog::spacingHint());
	buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	// Add Server
	m_addServerButton = new KPushButton( KGuiItem(i18n("Add Server"), "bookmark_add"), this);
	buttonBox->addWidget(m_addServerButton);

	connect(m_addServerButton, SIGNAL(clicked()), this, SLOT(slotAddServer()));

	// Server List / Refresh
	m_refreshButton = new KPushButton( KGuiItem(useMonopigatorOnStart ? i18n("Refresh") : i18n("Server List"), useMonopigatorOnStart ? "reload" : "network"), this);
	buttonBox->addWidget(m_refreshButton);

	connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(slotRefresh()));

	// Connect
	m_connectButton = new KPushButton(BarIcon("forward", KIcon::SizeSmall), i18n("Connect"), this);
	m_connectButton->setEnabled(false);
	buttonBox->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(clicked()), this, SLOT(slotConnect()));

    // Status indicator
	status_label = new QLabel(this);
	m_mainLayout->addWidget(status_label);

	// Monopigator
	m_monopigator = new Monopigator();

	connect(m_monopigator, SIGNAL(monopigatorAdd(QString, QString, QString, int)), this, SLOT(slotMonopigatorAdd(QString, QString, QString, int)));
	connect(m_monopigator, SIGNAL(finished()), SLOT(monopigatorFinished()));
	connect(m_monopigator, SIGNAL(timeout()), SLOT(monopigatorTimeout()));

	// Until we have a good way to use start a local monopd server, disable this button
//	m_localGameButton->setEnabled(false);

	slotRefresh(useMonopigatorOnStart);
}

SelectServer::~SelectServer()
{
	delete m_monopigator;
	delete m_localSocket;
}

void SelectServer::initMonopigator()
{
	// Hardcoded, but there aren't any other Monopigator root servers at the moment
	status_label->setText(i18n("Retrieving server list..."));
	m_refreshButton->setGuiItem(KGuiItem(i18n("Refresh"), "reload"));
	m_monopigator->loadData("http://gator.monopd.net/");
}

void SelectServer::checkCustomServer(const QString &host, int port)
{
	m_localSocket = new KExtendedSocket(0, 0, KExtendedSocket::inputBufferedSocket);
	connect(m_localSocket, SIGNAL(connectionSuccess()), this, SLOT(slotCustomConnected()));
	connect(m_localSocket, SIGNAL(connectionFailed(int)), this, SLOT(slotCustomError()));
	m_localSocket->setAddress(host, port);
	m_localSocket->enableRead(true);
	m_localSocket->startAsyncConnect();
}

void SelectServer::slotMonopigatorAdd(QString host, QString port, QString version, int users)
{
	QListViewItem *item = new QListViewItem(m_serverList, host, version, (users == -1) ? i18n("unknown") : QString::number(users), port);
	item->setPixmap(0, BarIcon("atlantik", KIcon::SizeSmall));
	validateConnectButton();
}

void SelectServer::slotCustomConnected()
{
	m_localServerAvailable = true;

	QListViewItem *item = new QListViewItem(m_serverList, m_localSocket->host(), i18n("unknown"), i18n("unknown"), m_localSocket->port());
	item->setPixmap(0, BarIcon("atlantik", KIcon::SizeSmall));

	validateConnectButton();
}

void SelectServer::slotCustomError()
{
	m_localServerAvailable = false;

	// TODO: fork server or offer option to do so
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

void SelectServer::slotRefresh(bool useMonopigator)
{
	m_localServerAvailable = false;

	m_serverList->clear();
	validateConnectButton();

	checkCustomServer("localhost", 1234);
	if (useMonopigator)
	{
		m_refreshButton->setEnabled(false);
		initMonopigator();
	}
}

void SelectServer::slotAddServer()
{
	KLineEditDlg dlg(i18n("Host:"), "", 0);
	dlg.setCaption(i18n("Add monopd Server"));
	dlg.enableButtonOK(false); // text is empty by default
	if (!dlg.exec())
		return;

	checkCustomServer(dlg.text(), 1234);
}

void SelectServer::slotConnect()
{
	if (QListViewItem *item = m_serverList->selectedItem())
		emit serverConnect(item->text(0), item->text(3).toInt());
}
