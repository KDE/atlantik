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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef ATLANTIK_PORTFOLIOESTATE_H
#define ATLANTIK_PORTFOLIOESTATE_H

#include <qpixmap.h>
#include <qwidget.h>

#define PE_WIDTH	13
#define	PE_HEIGHT	16

class Estate;
class Player;
  
class PortfolioEstate : public QWidget
{
Q_OBJECT

public:
	PortfolioEstate(Estate *estate, Player *player, bool alwaysOwned, QWidget *parent, const char *name = 0);
	Estate *estate() { return m_estate; }
	static QPixmap drawPixmap(Estate *estate, Player *player = 0, bool alwaysOwned = true);

protected:
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *);

private slots:
	void estateChanged();

signals:
	void estateClicked(Estate *estate);

private:
	Estate *m_estate;
	Player *m_player;
	QPixmap m_pixmap;
	bool b_recreate, m_alwaysOwned;
};

#endif
