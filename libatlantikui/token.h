// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
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

#ifndef ATLANTIK_TOKEN_H
#define ATLANTIK_TOKEN_H

#include <qwidget.h>
#include <qpixmap.h>

class Player;
class Estate;
class EstateView;
class AtlantikBoard;

class Token : public QWidget
{
Q_OBJECT

	public:
		Token (Player *player, EstateView *location, AtlantikBoard *parent, const char *name = 0);
		Player *player();
		void setLocation(EstateView *estateView, bool confirm = true);
		EstateView *location() { return m_location; }
		void setDestination(EstateView *estateView);
		EstateView *destination() { return m_destination; }
		void updateGeometry();

	private slots:
		void playerChanged();

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);

signals:
	void tokenConfirmation(Estate *);

private:
		Player *m_player;
		EstateView *m_location, *m_destination;
		AtlantikBoard *m_parentBoard;
		bool b_recreate, m_inJail;
		QPixmap *qpixmap;
};

#endif
