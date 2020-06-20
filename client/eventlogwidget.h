// Copyright (c) 2003 Rob Kaper <cap@capsi.com>
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

#ifndef ATLANTIK_EVENTLOGWIDGET_H
#define ATLANTIK_EVENTLOGWIDGET_H

#include <network_defs.h>

#include <QSortFilterProxyModel>
#include <QList>
#include <QDialog>
#include <QIcon>

class QString;

class Event;

class QTextStream;
class QTreeView;

class EventLog : public QAbstractItemModel
{
Q_OBJECT

public:
	EventLog(QObject *parent = nullptr);
	~EventLog();

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

public Q_SLOTS:
	void addEvent(const QString &description, EventType type);
	void clear();
	void saveAsText(QTextStream &stream) const;

private:
	QIcon cachedIcon(EventType type) const;

	QList<Event*> m_events;
	mutable QIcon m_iconCache[ET_LastEvent];
};

class LastMessagesProxyModel : public QSortFilterProxyModel
{
Q_OBJECT

public:
	LastMessagesProxyModel(QObject *parent = nullptr);

	void setMessagesCount(int n);

	void setSourceModel(QAbstractItemModel *model) override;

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
	int m_count;
};

class EventLogWidget : public QDialog
{
Q_OBJECT

public:
	EventLogWidget(EventLog *eventLog, QWidget *parent=nullptr);
	~EventLogWidget();
	void restoreSettings();

private Q_SLOTS:
	void save();

private:
	EventLog *m_eventLog;
	QTreeView *m_eventList;
};

#endif
