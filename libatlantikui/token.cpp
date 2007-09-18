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

#include <iostream>

#include <qpainter.h>
#include <qpixmap.h>
#include <qfont.h>

#include <kdebug.h>

#include <kstandarddirs.h>
#include <kglobalsettings.h>

#include "board.h"
#include "estate.h"
#include "player.h"

#include "token.moc"

#define TOKEN_ICONSIZE	32

Token::Token(Player *player, AtlantikBoard *parent, const char *name) : QWidget(parent, name)
{
	setBackgroundMode(NoBackground); // avoid flickering

	m_parentBoard = parent;

	m_player = player;
	connect(m_player, SIGNAL(changed(Player *)), this, SLOT(playerChanged()));

	m_inJail = m_player->inJail();
	m_location = m_player->location();
	m_destination = 0;

	qpixmap = 0;
	b_recreate = true;

	// Init icon
	m_image = 0;
	loadIcon();

	setFixedSize(QSize(TOKEN_ICONSIZE, TOKEN_ICONSIZE + KGlobalSettings::generalFont().pointSize()));
}

Token::~Token()
{
	delete m_image;
}

Player *Token::player()
{
	return m_player;
}

void Token::setLocation(Estate *location)
{
	if (m_location != location)
		m_location = location;
}

void Token::setDestination(Estate *estateView)
{
	if (m_destination != estateView)
		m_destination = estateView;
}

void Token::playerChanged()
{
	if (m_imageName != m_player->image())
		loadIcon();

	b_recreate = true;
	update();
}

void Token::loadIcon()
{
	m_imageName = m_player->image();

	delete m_image;
	m_image = 0;

	if (!m_imageName.isEmpty())
	{
		QString filename = locate("data", "atlantik/themes/default/tokens/" + m_imageName);
		if (KStandardDirs::exists(filename))
			m_image = new QPixmap(filename);
	}

	if (!m_image)
	{
		m_imageName = "hamburger.png";

		QString filename = locate("data", "atlantik/themes/default/tokens/" + m_imageName);
		if (KStandardDirs::exists(filename))
			m_image = new QPixmap(filename);
	}

	QWMatrix m;
	m.scale(double(TOKEN_ICONSIZE) / m_image->width(), double(TOKEN_ICONSIZE) / m_image->height());
	QPixmap *scaledPixmap = new QPixmap(TOKEN_ICONSIZE, TOKEN_ICONSIZE);
	*scaledPixmap = m_image->xForm(m);

	delete m_image;
	m_image = scaledPixmap;
}

void Token::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QPainter painter;
		painter.begin(qpixmap, this);

        if (m_image)
		{
			painter.setPen(Qt::black);
			painter.setBrush(Qt::white);
			painter.drawRect(0, 0, TOKEN_ICONSIZE, TOKEN_ICONSIZE);

			painter.drawPixmap(0, 0, *m_image);
		}

		painter.setPen(Qt::black);
		painter.setBrush(Qt::black);
		painter.drawRect(0, TOKEN_ICONSIZE, width(), KGlobalSettings::generalFont().pointSize());

		painter.setPen(Qt::white);
		painter.setFont(QFont(KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize(), QFont::DemiBold));
		painter.drawText(1, height()-1, (m_player ? m_player->name() : QString::null));

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void Token::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
}
