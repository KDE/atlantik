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

#include <QTcpSocket>
#include <QHeaderView>
#include <QIntValidator>

#include <klocalizedstring.h>
#include <kiconloader.h>
#include <kguiitem.h>

#include <monopd.h>

#include "metatlantic.h"

#include "selectserver_widget.h"

#include <atlantik_debug.h>

MetaserverEntry::MetaserverEntry(const QString &host, int port, const QString &version, int users) : QObject(), QTreeWidgetItem(MetaserverType), m_latency(9999), m_users(users), m_port(port)
{
	setText(0, host);
	setText(1, QString::number(m_latency));
	setText(2, version);
	setText(3, users == -1 ? i18n("unknown") : QString::number(m_users));
	setIcon(0, KDE::icon("atlantik"));
	setDisabled(true);

	m_isDev = version.indexOf("CVS") != -1 || version.indexOf("-dev") != -1;

	m_latencySocket = new QTcpSocket();
	m_latencySocket->setSocketOption(QAbstractSocket::LowDelayOption, true);
	connect(m_latencySocket, SIGNAL(hostFound()), this, SLOT(resolved()));
	connect(m_latencySocket, SIGNAL(connected()), this, SLOT(connected()));

	m_latencySocket->connectToHost(host, m_port);
}

bool MetaserverEntry::isDev() const
{
	return m_isDev;
}

QString MetaserverEntry::host() const
{
	return text(0);
}

int MetaserverEntry::port() const
{
	return m_port;
}

bool MetaserverEntry::operator<(const QTreeWidgetItem &other) const
{
	if (other.type() != MetaserverType)
		return QTreeWidgetItem::operator<(other);

	// sort by (less) latency first, then (most) users, then by host
	const MetaserverEntry *e = static_cast<const MetaserverEntry *>(&other);
	if (m_latency < e->m_latency)
		return true;
	if (m_users > e->m_users)
		return true;
	return QString::compare(host(), e->host()) > 0;
}

void MetaserverEntry::resolved()
{
	m_timer.start();
	qCDebug(ATLANTIK_LOG) << host() << "resolved; timer starts";
}

void MetaserverEntry::connected()
{
	m_latency = m_timer.elapsed();
	m_latencySocket->abort();
	qCDebug(ATLANTIK_LOG) << "connected to" << host() << "- latency =" << m_latency;
	setDisabled(false);
	setText(1, QString::number(m_latency));
	disconnect(m_latencySocket, 0, this, 0);
	m_latencySocket->deleteLater();
	m_latencySocket = 0;
}

void MetaserverEntry::showDevelopmentServers(bool show)
{
	setHidden(!show);
}

SelectServer::SelectServer(bool useMonopigatorOnStart, bool hideDevelopmentServers, QWidget *parent)
    : QWidget(parent)
{
	m_hideDevelopmentServers = hideDevelopmentServers;

	setupUi(this);
	layout()->setMargin(0);

	connect(m_hostEdit, SIGNAL(returnPressed()), this, SLOT(customConnect()));

	m_portEdit->setText(QString::number(MONOPD_PORT));
	m_portEdit->setValidator(new QIntValidator(1, 65535, m_portEdit));
	connect(m_portEdit, SIGNAL(returnPressed()), this, SLOT(customConnect()));

	connectButton->setIcon(KDE::icon("network-wired"));
	connect(connectButton, SIGNAL(clicked()), this, SLOT(customConnect()));

	// List of servers
	m_serverList->sortItems(1, Qt::AscendingOrder);
	m_serverList->header()->setClickable(false);
	m_serverList->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	m_serverList->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	m_serverList->header()->setResizeMode(3, QHeaderView::ResizeToContents);

	connect(m_serverList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(validateConnectButton()));
	connect(m_serverList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slotConnect()));
	connect(m_serverList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(validateConnectButton()));

	// Server List / Refresh
	KGuiItem::assign(m_refreshButton, useMonopigatorOnStart ? KGuiItem(i18n("Reload Server List"), "view-refresh") : KGuiItem(i18n("Get Server List"), "network-wired"));
	connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(slotRefresh()));

	// Connect
	m_connectButton->setIcon(KDE::icon("go-next"));

	connect(m_connectButton, SIGNAL(clicked()), this, SLOT(slotConnect()));

	// Monopigator
	m_metatlantic = new Metatlantic();

	connect(m_metatlantic, SIGNAL(metatlanticAdd(QString,int,QString,int)), this, SLOT(slotMetatlanticAdd(QString,int,QString,int)));
	connect(m_metatlantic, SIGNAL(finished()), SLOT(metatlanticFinished()));
	connect(m_metatlantic, SIGNAL(timeout()), SLOT(metatlanticTimeout()));
}

SelectServer::~SelectServer()
{
	delete m_metatlantic;
}

void SelectServer::setHideDevelopmentServers(bool hideDevelopmentServers)
{
	if ( m_hideDevelopmentServers != hideDevelopmentServers )
	{
		m_hideDevelopmentServers = hideDevelopmentServers;
		emit showDevelopmentServers( !m_hideDevelopmentServers );
	}
}

void SelectServer::initMonopigator()
{
	// Hardcoded, but there aren't any other Metatlantic root servers at the moment
	emit msgStatus(i18n("Retrieving server list..."));

	KGuiItem::assign(m_refreshButton, KGuiItem(i18n("Reload Server List"), "view-refresh"));
	m_metatlantic->loadData(METATLANTIC_HOST, METATLANTIC_PORT);
}

void SelectServer::slotMetatlanticAdd(const QString &host, int port, const QString &version, int users)
{
	MetaserverEntry *item = new MetaserverEntry(host, port, version, users);
	m_serverList->addTopLevelItem(item);
	m_serverList->resizeColumnToContents(0);

	if ( item->isDev() )
	{
		item->setHidden(m_hideDevelopmentServers);
		connect(this, SIGNAL(showDevelopmentServers(bool)), item, SLOT(showDevelopmentServers(bool)));
	}

	validateConnectButton();
}

void SelectServer::metatlanticFinished()
{
	emit msgStatus(i18n("Retrieved server list."));
	m_refreshButton->setEnabled(true);
}

void SelectServer::metatlanticTimeout()
{
	emit msgStatus(i18n("Error while retrieving the server list."));
	m_refreshButton->setEnabled(true);
}

void SelectServer::validateConnectButton()
{
	if (!m_serverList->selectedItems().isEmpty())
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
	const QList<QTreeWidgetItem *> items = m_serverList->selectedItems();
	if (!items.isEmpty())
	{
		const QTreeWidgetItem *item = items.first();
		Q_ASSERT(item->type() == MetaserverEntry::MetaserverType);
		const MetaserverEntry *e = static_cast<const MetaserverEntry *>(item);
		emit serverConnect(e->host(), e->port());
	}
}

void SelectServer::customConnect()
{
	if (!m_hostEdit->text().isEmpty() && !m_portEdit->text().isEmpty())
		emit serverConnect(m_hostEdit->text(), m_portEdit->text().toInt());
}
