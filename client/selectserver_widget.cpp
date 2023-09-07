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
#include <kstandardguiitem.h>

#include <monopd.h>

#include "metatlantic.h"

#include "selectserver_widget.h"

#include <atlantik_debug.h>

MetaserverEntry::MetaserverEntry(const QString &host, int port, const QString &version, int users)
	: QObject()
	, QTreeWidgetItem(MetaserverType)
	, m_latency(9999)
	, m_users(users)
	, m_port(port)
	, m_isDev(version.indexOf(QLatin1String("CVS")) != -1 || version.indexOf(QLatin1String("-dev")) != -1)
{
	setText(0, host);
	setText(1, QString::number(m_latency));
	setText(2, version);
	setText(3, users == -1 ? i18n("unknown") : QString::number(m_users));
	setIcon(0, KDE::icon(QStringLiteral("atlantik")));
	setDisabled(true);

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
	disconnect(m_latencySocket, nullptr, this, nullptr);
	m_latencySocket->deleteLater();
	m_latencySocket = nullptr;
}

SelectServer::SelectServer(bool hideDevelopmentServers, QWidget *parent)
	: QWidget(parent)
	, m_metatlantic(nullptr)
	, m_hideDevelopmentServers(hideDevelopmentServers)
{
	setupUi(this);
	layout()->setContentsMargins(0, 0, 0, 0);

	connect(m_hostEdit, SIGNAL(returnPressed()), this, SLOT(customConnect()));
	connect(m_hostEdit, SIGNAL(textChanged(QString)), this, SLOT(validateCustomConnectButton()));

	m_portEdit->setText(QString::number(MONOPD_PORT));
	m_portEdit->setValidator(new QIntValidator(1, 65535, m_portEdit));
	connect(m_portEdit, SIGNAL(returnPressed()), this, SLOT(customConnect()));
	connect(m_portEdit, SIGNAL(textChanged(QString)), this, SLOT(validateCustomConnectButton()));

	m_customConnectButton->setIcon(KDE::icon(QStringLiteral("network-wired")));
	connect(m_customConnectButton, SIGNAL(clicked()), this, SLOT(customConnect()));

	// List of servers
	m_serverList->sortItems(1, Qt::AscendingOrder);
	m_serverList->header()->setSectionsClickable(false);
	m_serverList->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	m_serverList->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	m_serverList->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

	connect(m_serverList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(validateConnectButton()));
	connect(m_serverList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slotConnect()));
	connect(m_serverList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(validateConnectButton()));

	// Server List / Refresh
	KGuiItem::assign(m_refreshButton, KGuiItem(i18n("Get Server List"), QStringLiteral("network-wired")));
	connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(reloadServerList()));

	// Connect
	m_connectButton->setIcon(KStandardGuiItem::forward(KStandardGuiItem::UseRTL).icon());

	connect(m_connectButton, SIGNAL(clicked()), this, SLOT(slotConnect()));

	validateConnectButton();
	validateCustomConnectButton();
}

SelectServer::~SelectServer()
{
	if (m_metatlantic)
		m_metatlantic->kill();
	delete m_metatlantic;
}

void SelectServer::setHideDevelopmentServers(bool hideDevelopmentServers)
{
	if ( m_hideDevelopmentServers != hideDevelopmentServers )
	{
		m_hideDevelopmentServers = hideDevelopmentServers;
		foreach (MetaserverEntry *e, m_developmentServers)
			e->setHidden(m_hideDevelopmentServers);
	}
}

void SelectServer::initMetaserver()
{
	// Hardcoded, but there aren't any other Metatlantic root servers at the moment
	Q_EMIT msgStatus(i18n("Retrieving server list..."));

	KGuiItem::assign(m_refreshButton, KGuiItem(i18n("Reload Server List"), QStringLiteral("view-refresh")));
	m_metatlantic = new Metatlantic(QString::fromLatin1(METATLANTIC_HOST), METATLANTIC_PORT);

	connect(m_metatlantic, SIGNAL(metatlanticAdd(QString,int,QString,int)), this, SLOT(slotMetatlanticAdd(QString,int,QString,int)));
	connect(m_metatlantic, SIGNAL(result(KJob*)), SLOT(metatlanticFinished()));

	m_metatlantic->start();
}

void SelectServer::slotMetatlanticAdd(const QString &host, int port, const QString &version, int users)
{
	MetaserverEntry *item = new MetaserverEntry(host, port, version, users);
	m_serverList->addTopLevelItem(item);
	m_serverList->resizeColumnToContents(0);

	if ( item->isDev() )
	{
		m_developmentServers.append(item);
		item->setHidden(m_hideDevelopmentServers);
	}

	validateConnectButton();
}

void SelectServer::metatlanticFinished()
{
	if (m_metatlantic->error())
		Q_EMIT msgStatus(i18n("Error while retrieving the server list."));
	else
		Q_EMIT msgStatus(i18n("Retrieved server list."));
	m_metatlantic = nullptr;
	m_refreshButton->setEnabled(true);
}

void SelectServer::validateConnectButton()
{
	if (!m_serverList->selectedItems().isEmpty())
		m_connectButton->setEnabled(true);
	else
		m_connectButton->setEnabled(false);
}

void SelectServer::validateCustomConnectButton()
{
	m_customConnectButton->setEnabled(!m_hostEdit->text().isEmpty() && !m_portEdit->text().isEmpty());
}

void SelectServer::reloadServerList()
{
	m_developmentServers.resize(0); // Keep the capacity allocated
	m_serverList->clear();
	validateConnectButton();

	m_refreshButton->setEnabled(false);
	initMetaserver();
}

void SelectServer::slotConnect()
{
	const QList<QTreeWidgetItem *> items = m_serverList->selectedItems();
	if (!items.isEmpty())
	{
		const QTreeWidgetItem *item = items.first();
		Q_ASSERT(item->type() == MetaserverEntry::MetaserverType);
		const MetaserverEntry *e = static_cast<const MetaserverEntry *>(item);
		Q_EMIT serverConnect(e->host(), e->port());
	}
}

void SelectServer::customConnect()
{
	if (!m_hostEdit->text().isEmpty() && !m_portEdit->text().isEmpty())
		Q_EMIT serverConnect(m_hostEdit->text(), m_portEdit->text().toInt());
}

#include "moc_selectserver_widget.cpp"
