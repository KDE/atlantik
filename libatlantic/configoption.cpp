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

ConfigOption::ConfigOption(int configId)
	: QObject()
	, m_id(configId)
	, m_changed(false)
	, m_edit(false)
{
}

int ConfigOption::id() const
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

bool ConfigOption::edit() const
{
	return m_edit;
}

void ConfigOption::setType(const QString &type)
{
	if (m_type != type)
	{
		m_type = type;
		m_changed = true;
	}
}

QString ConfigOption::type() const
{
	return m_type;
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
		Q_EMIT changed(this);
		m_changed = false;
	}
}

#include "moc_configoption.cpp"
