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

#include <qwidget.h>
#include <qmap.h>

class QString;

class Event;

class KListView;
class KListViewItem;
class KPushButton;

class EventLog : public QObject
{
Q_OBJECT

public:
	EventLog();
	QPtrList<Event> events();

public slots:
	void addEvent(const QString &description, const QString &icon = QString::null);

signals:
	void newEvent(Event *event);

private:
	QPtrList<Event> m_events;
};

class EventLogWidget : public QWidget
{
Q_OBJECT

public:
	enum EventLogType { Default, Net_In, Net_Out };

	EventLogWidget(EventLog *eventLog, QWidget *parent=0, const char *name = 0);

public slots:
	void addEvent(Event *event);

protected:
	void closeEvent(QCloseEvent *e);

private slots:
	void save();

private:
	EventLog *m_eventLog;
	KListView *m_eventList;
	KPushButton *m_saveButton;
};

#endif
