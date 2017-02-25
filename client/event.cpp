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

#include "event.h"

Event::Event(const QDateTime &dateTime, const QString &description, EventType type)
	: m_dateTime(dateTime)
	, m_description(description)
	, m_type(type >= 0 && type < ET_LastEvent ? type : ET_Generic)
{
}

QDateTime Event::dateTime() const
{
	return m_dateTime;
}

QString Event::description() const
{
	return m_description;
}

EventType Event::type() const
{
	return m_type;
}
