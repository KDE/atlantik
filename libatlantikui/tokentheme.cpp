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

struct TokenThemeFiller
{
	static void iterateThemes(void *data, void (*fun)(void *, const QString &, const QString &));
	static void fillNames(void *data, const QString &, const QString &name);
	static void createTheme(void *data, const QString &dir, const QString &name);
	static void getSpecificTheme(void *data, const QString &dir, const QString &name);
};

struct SpecificThemeData
{
	QString name;
	TokenTheme theme;
};

void TokenThemeFiller::iterateThemes(void *data, void (*fun)(void *, const QString &, const QString &))
{
	foreach (const QString &dir, QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "atlantik/themes/", QStandardPaths::LocateDirectory))
	{
		QDirIterator it(dir, QDir::Dirs | QDir::NoDotAndDotDot);
		while (it.hasNext())
		{
			QString d = it.next();
			const QString fn = it.fileName();
			d += "/tokens/";
			if (QFileInfo(d).isDir())
				fun(data, d, fn);
		}
	}
}

void TokenThemeFiller::fillNames(void *data, const QString &, const QString &name)
{
	static_cast<QSet<QString> *>(data)->insert(name);
}

void TokenThemeFiller::createTheme(void *data, const QString &dir, const QString &name)
{
	static_cast<QVector<TokenTheme> *>(data)->append(TokenTheme(name, dir));
}

void TokenThemeFiller::getSpecificTheme(void *data, const QString &dir, const QString &name)
{
	SpecificThemeData &d = *static_cast<SpecificThemeData *>(data);
	if (name == d.name)
		d.theme = TokenTheme(name, dir);
}

TokenTheme::TokenTheme()
{
}

TokenTheme::TokenTheme(const QString &name, const QString &dir)
	: m_name(name), m_dir(dir)
{
}

TokenTheme::TokenTheme(const TokenTheme &other)
	: m_name(other.m_name), m_dir(other.m_dir)
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
	return "hamburger.png";
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

	TokenThemeFiller::iterateThemes(&set, &TokenThemeFiller::fillNames);

	return set.toList();
}

QVector<TokenTheme> TokenTheme::themes()
{
	QVector<TokenTheme> list;

	TokenThemeFiller::iterateThemes(&list, &TokenThemeFiller::createTheme);

	return list;
}

TokenTheme TokenTheme::theme(const QString &theme)
{
	SpecificThemeData data;
	data.name = theme;

	TokenThemeFiller::iterateThemes(&data, &TokenThemeFiller::getSpecificTheme);

	return data.theme;
}
