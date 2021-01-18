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

#include "tokentheme.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSet>
#include <QStandardPaths>

#include <functional>

static void iterateTokenThemes(std::function<void(const QString &, const QString &)> fun)
{
	foreach (const QString &dir, QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QLatin1String("atlantik/themes/"), QStandardPaths::LocateDirectory))
	{
		QDirIterator it(dir, QDir::Dirs | QDir::NoDotAndDotDot);
		while (it.hasNext())
		{
			QString d = it.next();
			const QString fn = it.fileName();
			d += QLatin1String("/tokens/");
			if (QFileInfo(d).isDir())
				fun(d, fn);
		}
	}
}

TokenTheme::TokenTheme()
{
}

TokenTheme::TokenTheme(const QString &name, const QString &dir)
	: m_name(name)
	, m_dir(dir)
{
}

TokenTheme::TokenTheme(const TokenTheme &other)
	: m_name(other.m_name)
	, m_dir(other.m_dir)
{
}

TokenTheme::~TokenTheme()
{
}

TokenTheme &TokenTheme::operator=(const TokenTheme &other)
{
	m_name = other.m_name;
	m_dir = other.m_dir;
	return *this;
}

bool TokenTheme::isValid() const
{
	return !m_dir.isEmpty();
}

QString TokenTheme::name() const
{
	return m_name;
}

QString TokenTheme::path() const
{
	return m_dir;
}

QString TokenTheme::fallbackIcon() const
{
	return QStringLiteral("hamburger.png");
}

QString TokenTheme::tokenPath(const QString &name) const
{
	if (!isValid())
		return QString();

	const QString filename = m_dir + name;
	if (!QFile::exists(filename))
		return QString();

	return filename;
}

QPixmap TokenTheme::tokenPixmap(const QString &name) const
{
	const QString path = tokenPath(name);
	return path.isEmpty() ? QPixmap() : QPixmap(path);
}

QStringList TokenTheme::themeNames()
{
	QSet<QString> set;

	iterateTokenThemes(
		[&set](const QString &, const QString &name)
		{
			set.insert(name);
		}
	);

	return set.values();
}

QVector<TokenTheme> TokenTheme::themes()
{
	QVector<TokenTheme> list;

	iterateTokenThemes(
		[&list](const QString &dir, const QString &name)
		{
			list.append(TokenTheme(name, dir));
		}
	);

	return list;
}

TokenTheme TokenTheme::theme(const QString &theme)
{
	TokenTheme newTheme;

	iterateTokenThemes(
		[&newTheme, theme](const QString &dir, const QString &name)
		{
			if (name == theme)
				newTheme = TokenTheme(name, dir);
		}
	);

	return newTheme;
}
