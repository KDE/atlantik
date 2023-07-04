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

#include <qpainter.h>
#include <qpixmap.h>
#include <QFont>
#include <QFile>
#include <QFontDatabase>
#include <QPaintEvent>

#include "board.h"
#include "estate.h"
#include "player.h"

#include "token.h"

#define ICONSIZE	32

Token::Token(Player *player, AtlantikBoard *parent)
	: QWidget(parent)
	, m_player(player)
	, m_location(m_player->location())
	, m_destination(nullptr)
	, m_inJail(m_player->inJail())
	, m_parentBoard(parent)
	, b_recreate(true)
	, qpixmap(nullptr)
	, m_image(nullptr)
	, m_playerName(m_player->name())
{
        setAttribute(Qt::WA_NoSystemBackground, true);

	connect(m_player, SIGNAL(changed(Player *)), this, SLOT(playerChanged()));

	// Init icon
	loadIcon();

	setFixedSize(QSize(ICONSIZE, ICONSIZE + QFontDatabase::systemFont(QFontDatabase::GeneralFont).pointSize()));
}

Token::~Token()
{
	delete m_image;
	delete qpixmap;
}

Player *Token::player() const
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

void Token::setTokenTheme(const TokenTheme &theme)
{
	m_theme = theme;
	loadIcon();
	b_recreate = true;
	update();
}

void Token::playerChanged()
{
	if (m_imageName != m_player->image())
	{
		loadIcon();
		b_recreate = true;
	}
	if (m_playerName != m_player->name())
	{
		m_playerName = m_player->name();
		b_recreate = true;
	}

	if (b_recreate)
		update();
}

void Token::loadIcon()
{
	m_imageName = m_player->image();

	delete m_image;
	m_image = nullptr;

	if (!m_theme.isValid())
		return;

	QString imageFile;
	if (!m_imageName.isEmpty())
		imageFile = m_theme.tokenPath(m_imageName);
	if (imageFile.isEmpty())
		imageFile = m_theme.tokenPath(m_theme.fallbackIcon());

	const QPixmap pix(imageFile);
	if (pix.isNull())
		return;

	m_image = new QPixmap(pix.scaled(ICONSIZE, ICONSIZE, Qt::KeepAspectRatio));
}

void Token::paintEvent(QPaintEvent *e)
{
	if (b_recreate)
	{
		const QFont generalFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
		if (!qpixmap || qpixmap->size() != size())
		{
			delete qpixmap;
			qpixmap = new QPixmap(width(), height());
		}

		QPainter painter;
		painter.begin(qpixmap );

        if (m_image)
		{
			painter.setPen(Qt::black);
			painter.setBrush(Qt::white);
			painter.drawRect(0, 0, ICONSIZE-1, ICONSIZE-1);

			painter.drawPixmap((ICONSIZE - m_image->width()) / 2, (ICONSIZE - m_image->height()) / 2, *m_image);
		}

		painter.setPen(Qt::black);
		painter.setBrush(Qt::black);
		painter.drawRect(0, ICONSIZE, width()-1, generalFont.pointSize()-1);

		painter.setPen(Qt::white);
		painter.setFont(QFont(generalFont.family(), generalFont.pointSize(), QFont::DemiBold));
		painter.drawText(1, height()-1, m_playerName);

		b_recreate = false;
	}
	QPainter painter(this);
	painter.drawPixmap(e->rect(), *qpixmap, e->rect());
}

void Token::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
}

#include "moc_token.cpp"
