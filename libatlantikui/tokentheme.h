// Copyright (c) 2015 Pino Toscano <pino@kde.org>
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

#ifndef ATLANTIK_TOKENTHEME_H
#define ATLANTIK_TOKENTHEME_H

#include <QPixmap>
#include <QStringList>
#include <QVector>

#include "libatlantikui_export.h"

class LIBATLANTIKUI_EXPORT TokenTheme
{
public:
	TokenTheme();
	TokenTheme(const TokenTheme &other);
	~TokenTheme();
	TokenTheme &operator=(const TokenTheme &other);

	bool isValid() const;
	QString name() const;
	QString path() const;

	QString fallbackIcon() const;

	QString tokenPath(const QString &name) const;
	QPixmap tokenPixmap(const QString &name) const;

	static QStringList themeNames();
	static QVector<TokenTheme> themes();
	static TokenTheme theme(const QString &theme);
	static TokenTheme defaultTheme() { return theme(QLatin1String("default")); }

private:
	friend struct TokenThemeFiller;
	TokenTheme(const QString &name, const QString &dir);

	QString m_name;
	QString m_dir;
};

#endif
