// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include "estategroup.h"
#include "estategroup.moc"

EstateGroup::EstateGroup(const QString name) : QObject()
{
	m_name = name;
}

void EstateGroup::setName(const QString name)
{
	if (m_name != name)
	{
		m_name = name;
		m_changed = true;
	}
}

void EstateGroup::update(bool force)
{
	if (m_changed || force)
	{
		emit changed();
		m_changed = false;
	}
}