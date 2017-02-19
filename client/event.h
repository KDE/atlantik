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

#ifndef ATLANTIK_EVENT_H
#define ATLANTIK_EVENT_H

#include <network_defs.h>

#include <QDateTime>
#include <QString>

class Event
{
public:
	Event(const QDateTime &dateTime, const QString &description, EventType type);
	QDateTime dateTime() const;
	QString description() const;
	EventType type() const;

private:
	QDateTime m_dateTime;
	QString m_description;
	EventType m_type : 8;
};

#endif
