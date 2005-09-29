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

#ifndef ATLANTIK_PORTFOLIOVIEW_H
#define ATLANTIK_PORTFOLIOVIEW_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qptrlist.h>

#include "portfolioestate.h"
#include "libatlantikui_export.h"
class QColor;
class QString;

class AtlanticCore;
class Player;
class Estate;

class LIBATLANTIKUI_EXPORT PortfolioView : public QWidget
{
Q_OBJECT

public:
	PortfolioView(AtlanticCore *core, Player *_player, QColor activeColor, QColor inactiveColor, QWidget *parent, const char *name = 0);
	~PortfolioView();

	void buildPortfolio();
	void clearPortfolio();

	Player *player();

protected:
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	void mousePressEvent(QMouseEvent *);

signals:
	void newTrade(Player *player);
	void kickPlayer(Player *player);
	void estateClicked(Estate *);

private slots:
	void playerChanged();
	void slotMenuAction(int item);

private:
	void loadIcon();

	AtlanticCore *m_atlanticCore;
	Player *m_player;
	PortfolioEstate *m_lastPE;
	QColor m_activeColor, m_inactiveColor;
	QPixmap *qpixmap, *m_image;
	QString m_imageName;
	bool b_recreate;
	QPtrList<PortfolioEstate> m_portfolioEstates;
};

#endif
