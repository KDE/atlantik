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

#include <QList>

#include <kdialog.h>

class QString;

class Event;

class K3ListView;

class EventLog : public QObject
{
Q_OBJECT

public:
	EventLog();
	~EventLog();
	QList<Event*> events();

public slots:
	void addEvent(const QString &description, const QString &icon = QString::null);

signals:
	void newEvent(Event *event);

private:
	QList<Event*> m_events;
};

class EventLogWidget : public KDialog
{
Q_OBJECT

public:
	enum EventLogType { Default, Net_In, Net_Out };

	EventLogWidget(EventLog *eventLog, QWidget *parent=0);

public slots:
	void addEvent(Event *event);

protected:
	void closeEvent(QCloseEvent *e);

private slots:
	void save();

private:
	EventLog *m_eventLog;
	K3ListView *m_eventList;
};

#endif
