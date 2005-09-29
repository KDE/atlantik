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

#include "configoption.h"

ConfigOption::ConfigOption(int configId) : QObject()
{
	m_id = configId;
	m_name = "";
	m_description = "";
	m_edit = false;
	m_value = "";
	m_changed = false;
}

int ConfigOption::id()
{
	return m_id;
}

void ConfigOption::setName(const QString &name)
{
	if (m_name != name)
	{
		m_name = name;
		m_changed = true;
	}
}

QString ConfigOption::name() const
{
	return m_name;
}

void ConfigOption::setDescription(const QString &description)
{
	if (m_description != description)
	{
		m_description = description;
		m_changed = true;
	}
}

QString ConfigOption::description() const
{
	return m_description;
}

void ConfigOption::setEdit(bool edit)
{
	if (m_edit != edit)
	{
		m_edit = edit;
		m_changed = true;
	}
}

bool ConfigOption::edit()
{
	return m_edit;
}

void ConfigOption::setValue(const QString &value)
{
	if (m_value != value)
	{
		m_value = value;
		m_changed = true;
	}
}

QString ConfigOption::value() const
{
	return m_value;
}

void ConfigOption::update(bool force)
{
	if (m_changed || force)
	{
		emit changed(this);
		m_changed = false;
	}
}

#include "configoption.moc"
