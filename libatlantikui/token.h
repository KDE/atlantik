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

#ifndef ATLANTIK_TOKEN_H
#define ATLANTIK_TOKEN_H

#include <QWidget>

#include "tokentheme.h"

class QPixmap;

class Player;
class Estate;
class AtlantikBoard;

class Token : public QWidget
{
Q_OBJECT

	public:
		Token (Player *player, AtlantikBoard *parent);
		~Token();
		Player *player() const;
		void setLocation(Estate *estate);
		Estate *location() const { return m_location; }
		void setDestination(Estate *estate);
		Estate *destination() const { return m_destination; }
		void setInJail (bool inJail) { m_inJail = inJail; }
		bool inJail() const { return m_inJail; }
		void setTokenTheme(const TokenTheme &theme);

	private Q_SLOTS:
		void playerChanged();

	protected:
		void paintEvent(QPaintEvent *) override;
		void resizeEvent(QResizeEvent *) override;

private:
		void loadIcon();

		Player *m_player;
		Estate *m_location, *m_destination;
		bool m_inJail;
		AtlantikBoard *m_parentBoard;
		bool b_recreate;
		QPixmap *qpixmap, *m_image;
		QString m_imageName;
		QString m_playerName;
		TokenTheme m_theme;
};

#endif
