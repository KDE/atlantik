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

#include <klocalizedstring.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kicon.h>
#include <kpushbutton.h>

#include "event.h"
#include "eventlogwidget.moc"

EventLog::EventLog()
	: QAbstractItemModel()
{
}

EventLog::~EventLog()
{
	qDeleteAll(m_events);
}

void EventLog::addEvent(const QString &description, const QString &icon)
{
	const int oldCount = m_events.count();
	beginInsertRows(QModelIndex(), oldCount, oldCount);
	Event *event = new Event(QDateTime::currentDateTime(), description, icon);
	m_events.append(event);
	endInsertRows();
}

QList<Event*> EventLog::events()
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
			return e->icon().isEmpty() ? KIcon("atlantik") : KIcon(e->icon());
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
	: KDialog(parent,
	  Qt::WindowContextHelpButtonHint)
{
	setButtons(Close|User1);
	setDefaultButton(Close);
	setButtonGuiItem(User1, KGuiItem(i18n("&Save As..."), "document-save"));
	setModal(false);

	m_eventLog = eventLog;

	setWindowTitle(i18n("Event Log"));

	QVBoxLayout *listCompBox = new QVBoxLayout(mainWidget());
	listCompBox->setMargin(0);

	m_eventList = new QTreeView(mainWidget());
	m_eventList->setObjectName( "eventList" );
	listCompBox->addWidget(m_eventList);

	LastMessagesProxyModel *proxy = new LastMessagesProxyModel(m_eventList);
	// FIXME: allow a way to show older messages
	proxy->setMessagesCount(25);
	proxy->setSourceModel(m_eventLog);

	m_eventList->setModel(proxy);
	m_eventList->setRootIsDecorated(false);
	m_eventList->header()->setClickable(false);
	m_eventList->header()->setResizeMode(0, QHeaderView::ResizeToContents);

	connect(this, SIGNAL(user1Clicked()), this, SLOT(save()));
}

void EventLogWidget::closeEvent(QCloseEvent *e)
{
	e->accept();
}

void EventLogWidget::save()
{
	QFile file( KFileDialog::getSaveFileName() );
	if ( file.open( QIODevice::WriteOnly ) )
	{
		QTextStream stream(&file);

		stream << i18n( "Atlantik log file, saved at %1.", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") ) << endl;

		foreach (Event *e, m_eventLog->events())
			stream << e->dateTime().toString("yyyy-MM-dd hh:mm:ss") << " " << e->description() << endl;
		file.close();
	}
}
