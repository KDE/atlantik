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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

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

	setFixedSize(QSize(26, 26));
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
	b_recreate = true;
	update();
}

void Token::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(locate("data", "atlantik/pics/token.png"));

		QPainter painter;
		painter.begin(qpixmap, this);

		painter.drawPixmap(0, 0, *qpixmap);

		painter.setPen(Qt::black);
		painter.setFont(QFont(KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize(), QFont::Bold));
		painter.drawText(2, height()-2, m_player->name());

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void Token::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
}
