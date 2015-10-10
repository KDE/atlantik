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

#include <QSortFilterProxyModel>
#include <QList>

#include <kdialog.h>

class QString;

class Event;

class QTreeView;

class EventLog : public QAbstractItemModel
{
Q_OBJECT

public:
	EventLog(QObject *parent = 0);
	~EventLog();
	QList<Event*> events();

	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;

public slots:
	void addEvent(const QString &description, const QString &icon = QString::null);

private:
	QList<Event*> m_events;
};

class LastMessagesProxyModel : public QSortFilterProxyModel
{
Q_OBJECT

public:
	LastMessagesProxyModel(QObject *parent = 0);

	void setMessagesCount(int n);

	void setSourceModel(QAbstractItemModel *model);

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
	int m_count;
};

class EventLogWidget : public KDialog
{
Q_OBJECT

public:
	enum EventLogType { Default, Net_In, Net_Out };

	EventLogWidget(EventLog *eventLog, QWidget *parent=0);

protected:
	void closeEvent(QCloseEvent *e);

private slots:
	void save();

private:
	EventLog *m_eventLog;
	QTreeView *m_eventList;
};

#endif
