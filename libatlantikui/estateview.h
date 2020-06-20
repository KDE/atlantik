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

enum EstateOrientation { North=0, East=1, South=2, West=3 };

class QPixmap;

class Player;
class Estate;

class EstateView : public QWidget
{
Q_OBJECT

	public:
		EstateView(Estate *estate, EstateOrientation orientation, const QString &, 
			   bool indicateUnowned, bool highlightUnowned, bool darkenMortgaged, bool quartzEffects, QWidget *parent);
		~EstateView();
		void setViewProperties(bool indicateUnowned, bool highlightUnowned, bool darkenMortgaged, bool quartzEffects);
		Estate *estate() const { return m_estate; }
		void updatePE();
		EstateOrientation orientation() const { return m_orientation; }
		void setAllowEstateSales(bool allow);

	Q_SIGNALS:
		void estateToggleMortgage(Estate *estate);
		void estateHouseBuy(Estate *estate);
		void estateHouseSell(Estate *estate);
		void estateSell(Estate *estate);
		void newTrade(Player *player);
		void LMBClicked(Estate *estate);

	protected:
		void paintEvent(QPaintEvent *) override;
		void resizeEvent(QResizeEvent *) override;
		void contextMenuEvent(QContextMenuEvent *) override;
		void mousePressEvent(QMouseEvent *) override;

private:
	void updateToolTip();
		void loadIcon(const QString &_icon);

		QPixmap *rotatePixmap(QPixmap *) const;
		void drawQuartzBlocks(QPixmap *pi, const QColor &c1, const QColor &c2);

		Estate *m_estate;
		QPixmap *qpixmap, *icon;
		QPixmap *m_quartzBlocks, *m_pe;
		bool m_allowEstateSales : 1;
		bool m_indicateUnowned : 1, m_highlightUnowned : 1, m_darkenMortgaged : 1, m_quartzEffects : 1;
		bool b_recreate : 1, m_recreateQuartz : 1;
		EstateOrientation m_orientation : 3;
		int m_titleWidth, m_titleHeight;
		QString m_estateIcon;
		QColor m_estateColor;

	private Q_SLOTS:
		void estateChanged();
		void slotToggleMortgage();
		void slotHouseBuy();
		void slotHouseSell();
		void slotSell();
		void slotNewTrade();
};

#endif
