// Copyright (c) 2002-2004 Rob Kaper <cap@capsi.com>
// Copyright (c) 2015 Pino Toscano <pino@kde.org>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "customlocationiconbutton.h"

#include <QFile>
#include <QFileInfo>
#include <QVariant>

#include <kicondialog.h>
#include <kiconloader.h>

CustomLocationIconButton::CustomLocationIconButton(QWidget *parent)
	: QPushButton(parent)
{
	setProperty("kcfg_property", QByteArray("image"));

	connect(this, SIGNAL(clicked()), this, SLOT(slotChooseImage()));
}

CustomLocationIconButton::~CustomLocationIconButton()
{
}

void CustomLocationIconButton::setTokenTheme(const TokenTheme &theme)
{
	m_tokenTheme = theme;
}

void CustomLocationIconButton::setImage(const QString &image)
{
	if (!checkValid())
		return;

	const QPixmap p = m_tokenTheme.tokenPixmap(image);
	if (p.isNull())
		return;

	setIcon(QIcon(p));
	setIconSize(p.size());
	m_image = image;
}

QString CustomLocationIconButton::image() const
{
	return m_image;
}

void CustomLocationIconButton::slotChooseImage()
{
	if (!checkValid())
		return;

	KIconDialog iconDialog(this);
	iconDialog.setCustomLocation(m_tokenTheme.path());
	// begin with user icons, lock editing
	iconDialog.setup(KIconLoader::Desktop, KIconLoader::Application, false, 0, true, true, true);

	const QString image = iconDialog.openDialog();
	if (image.isEmpty())
		return;

	m_image = QFileInfo(image).fileName();

	setImage(m_image);
	Q_EMIT imageChanged(m_image);
}

bool CustomLocationIconButton::checkValid() const
{
	return m_tokenTheme.isValid();
}

#include "moc_customlocationiconbutton.cpp"
