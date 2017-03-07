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

#ifndef LIBATLANTIC_CONFIGOPTION_H
#define LIBATLANTIC_CONFIGOPTION_H

#include <QObject>

#include "libatlantic_export.h"

class LIBATLANTIC_EXPORT ConfigOption : public QObject
{
Q_OBJECT

public:
	ConfigOption(int configId);
	int id() const;
	void setName(const QString &name);
	QString name() const;
	void setDescription(const QString &description);
	QString description() const;
	void setEdit(bool edit);
	bool edit() const;
	void setType(const QString &type);
	QString type() const;
	void setValue(const QString &value);
	QString value() const;
	void update(bool force = false);

Q_SIGNALS:
	void changed(ConfigOption *configOption);

private:
	int m_id;
	bool m_changed : 1, m_edit : 1;
	QString m_name, m_description, m_type, m_value;
};

#endif
