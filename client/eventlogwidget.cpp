// Copyright (c) 2003-2004 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include <iostream>

#include <QLayout>
#include <QDateTime>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextStream>
#include <QCloseEvent>
#include <QTreeView>
#include <QHeaderView>
#include <QFileDialog>
#include <QPushButton>
#include <QDialogButtonBox>

#include <klocalizedstring.h>
#include <kiconloader.h>
#include <kguiitem.h>

#include "event.h"
#include "eventlogwidget.h"

EventLog::EventLog(QObject *parent)
	: QAbstractItemModel(parent)
{
}

EventLog::~EventLog()
{
	qDeleteAll(m_events);
}

void EventLog::addEvent(const QString &description, EventType type)
{
	const int oldCount = m_events.count();
	beginInsertRows(QModelIndex(), oldCount, oldCount);
	Event *event = new Event(QDateTime::currentDateTime(), description, type);
	m_events.append(event);
	endInsertRows();
}

QList<Event*> EventLog::events() const
{
	return m_events;
}

int EventLog::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 2;
}

QVariant EventLog::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= m_events.count()
	    || index.column() < 0 || index.column() > 1)
		return QVariant();

	Event *e = static_cast<Event *>(index.internalPointer());
	switch (index.column())
	{
	case 0:
		if (role != Qt::DisplayRole)
			return QVariant();
		return e->dateTime().toString("yyyy-MM-dd hh:mm:ss zzz");
	case 1:
		switch (role)
		{
		case Qt::DecorationRole:
			return cachedIcon(e->type());
		case Qt::DisplayRole:
			return e->description();
		}
		break;
	}

	return QVariant();
}

QVariant EventLog::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole
	    || section < 0 || section > 1)
		return QVariant();

	switch (section)
	{
	case 0:
		return i18n("Date/Time");
	case 1:
		return i18n("Description");
	}
	return QVariant();
}

QModelIndex EventLog::index(int row, int column, const QModelIndex &parent) const
{
	return parent.isValid() || row < 0 || row >= m_events.count()
	    || column < 0 || column > 1
	    ? QModelIndex()
	    : createIndex(row, column, m_events.at(row));
}

QModelIndex EventLog::parent(const QModelIndex &) const
{
	return QModelIndex();
}

int EventLog::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : m_events.count();
}

QIcon EventLog::cachedIcon(EventType type) const
{
	if (!m_iconCache[type].isNull())
		return m_iconCache[type];

	QString name;
	switch (type)
	{
		case ET_Generic:
			name = "atlantik";
			break;
		case ET_NetIn:
			name = "arrow-left";
			break;
		case ET_NetOut:
			name = "arrow-right";
			break;
		case ET_NetGeneric:
			name = "network-disconnect";  // FIXME
			break;
		case ET_NetConnected:
			name = "network-connect";
			break;
		case ET_NetError:
			name = "network-disconnect";
			break;
		case ET_LastEvent:
			return QIcon();
	};

	const QIcon icon = KDE::icon(name);
	m_iconCache[type] = icon;
	return icon;
}

LastMessagesProxyModel::LastMessagesProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent)
	, m_count(-1)
{
}

void LastMessagesProxyModel::setMessagesCount(int n)
{
	if (n == m_count)
		return;

	m_count = n;
	invalidateFilter();
}

void LastMessagesProxyModel::setSourceModel(QAbstractItemModel *model)
{
	if (sourceModel())
		disconnect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(invalidate()));
	QSortFilterProxyModel::setSourceModel(model);
	if (sourceModel())
		connect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(invalidate()));
}

bool LastMessagesProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if (m_count < 0)
		return true;
	return source_row + m_count >= sourceModel()->rowCount(source_parent);
}

EventLogWidget::EventLogWidget(EventLog *eventLog, QWidget *parent)
	: QDialog(parent,
	  Qt::WindowContextHelpButtonHint)
{
	setModal(false);

	m_eventLog = eventLog;

	setWindowTitle(i18n("Event Log"));

	QVBoxLayout *listCompBox = new QVBoxLayout(this);

	m_eventList = new QTreeView(this);
	m_eventList->setObjectName( "eventList" );
	listCompBox->addWidget(m_eventList);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
	QPushButton *saveButton = buttonBox->addButton(QString(), QDialogButtonBox::ActionRole);
	KGuiItem::assign(saveButton, KGuiItem(i18n("&Save As..."), "document-save-as"));
	listCompBox->addWidget(buttonBox);

	LastMessagesProxyModel *proxy = new LastMessagesProxyModel(m_eventList);
	// FIXME: allow a way to show older messages
	proxy->setMessagesCount(25);
	proxy->setSourceModel(m_eventLog);

	m_eventList->setModel(proxy);
	m_eventList->setRootIsDecorated(false);
	m_eventList->header()->setSectionsClickable(false);
	m_eventList->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

	connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));
	connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
}

void EventLogWidget::closeEvent(QCloseEvent *e)
{
	e->accept();
}

void EventLogWidget::save()
{
	const QString filename = QFileDialog::getSaveFileName(this);
	if (filename.isEmpty())
		return;

	QFile file(filename);
	if ( file.open( QIODevice::WriteOnly ) )
	{
		QTextStream stream(&file);

		stream << i18n( "Atlantik log file, saved at %1.", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") ) << endl;

		foreach (Event *e, m_eventLog->events())
			stream << e->dateTime().toString("yyyy-MM-dd hh:mm:ss") << " " << e->description() << endl;
		file.close();
	}
}
