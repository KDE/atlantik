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

#ifndef ATLANTIK_ESTATEVIEW_H
#define ATLANTIK_ESTATEVIEW_H

#include <QWidget>
#include <qpixmap.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>

#include <QPixmap>

#include "portfolioestate.h"

enum EstateOrientation { North=0, East=1, South=2, West=3 };

class Player;
class Estate;

class EstateView : public QWidget
{
Q_OBJECT

	public:
		EstateView(Estate *estate, EstateOrientation orientation, const QString &, 
			   bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects, QWidget *parent);
		~EstateView();
		void setViewProperties(bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects);
		Estate *estate() { return m_estate; }
		void updatePE();
		EstateOrientation orientation() { return m_orientation; }

	public slots:
		void slotResizeAftermath();

	signals:
		void estateToggleMortgage(Estate *estate);
		void estateHouseBuy(Estate *estate);
		void estateHouseSell(Estate *estate);
		void newTrade(Player *player);
		void LMBClicked(Estate *estate);

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);
		void mousePressEvent(QMouseEvent *);

private:
	void updateToolTip();

		QPixmap *rotatePixmap(QPixmap *);
		void drawQuartzBlocks(QPixmap *pi, const QColor &c1, const QColor &c2);
		void repositionPortfolioEstate();

		Estate *m_estate;
		QPixmap *qpixmap, *icon;
		QPixmap *m_quartzBlocks;
		bool m_indicateUnowned, m_highliteUnowned, m_darkenMortgaged, m_quartzEffects;
		bool b_recreate, m_recreateQuartz;
		int m_titleWidth, m_titleHeight;
		EstateOrientation m_orientation;
		PortfolioEstate *pe;

	private slots:
		void estateChanged();
		void slotToggleMortgage();
		void slotHouseBuy();
		void slotHouseSell();
		void slotNewTrade();
};

#endif
