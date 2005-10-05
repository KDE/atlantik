// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
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

#ifndef LIBATLANTIC_ESTATEGROUP_H
#define LIBATLANTIC_ESTATEGROUP_H

#include <qobject.h>

#include "libatlantic_export.h"

class LIBATLANTIC_EXPORT EstateGroup : public QObject
{
Q_OBJECT

public:
	EstateGroup(const int id);
	int id() { return m_id; }
	void setName(const QString name);
	QString name() const { return m_name; }
	void update(bool force = false);

signals:
	void changed();

private:
	int m_id;
	bool m_changed;
	QString m_name;
};

#endif
