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

#ifndef ATLANTIK_PORTFOLIOVIEW_H
#define ATLANTIK_PORTFOLIOVIEW_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qmap.h>

#include "portfolioestate.h"

class QColor;
class QString;

class AtlanticCore;
class Player;
class Estate;

class PortfolioView : public QWidget
{
Q_OBJECT

public:
	PortfolioView(AtlanticCore *core, Player *_player, QColor activeColor, QColor inactiveColor, QWidget *parent, const char *name = 0);
	void buildPortfolio();

	Player *player();

protected:
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	void mousePressEvent(QMouseEvent *);

signals:
	void newTrade(Player *player);
	void estateClicked(Estate *);

private slots:
	void playerChanged();
	void slotMenuAction(int item);

private:
	AtlanticCore *m_atlanticCore;
	Player *m_player;
	PortfolioEstate *m_lastPE;
	QColor m_activeColor, m_inactiveColor;
	QPixmap *qpixmap;
	bool b_recreate;
	QMap<int, PortfolioEstate*> portfolioEstateMap;
};

#endif
