#ifndef ATLANTIK_ESTATEVIEW_H
#define ATLANTIK_ESTATEVIEW_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qlabel.h>

#include <qtextview.h>

#include <kpixmap.h>

#include "portfolioestate.h"

enum Orientation{ North=0, East=1, South=2, West=3 };

class Estate;

class EstateView : public QWidget
{
Q_OBJECT

	public:
		EstateView(Estate *estate, int orientation, const QString &, QWidget *parent, const char *name = 0);
		void updatePE();

	public slots:
		void slotResizeAftermath();

	signals:
		void estateToggleMortgage(Estate *estate);
		void estateHouseBuy(Estate *estate);
		void estateHouseSell(Estate *estate);

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);
		void mousePressEvent(QMouseEvent *);

	private:
		QPixmap *rotatePixmap(QPixmap *);
		KPixmap *rotatePixmap(KPixmap *);
		void drawQuartzBlocks(KPixmap *pi, KPixmap &p, const QColor &c1, const QColor &c2);
		void repositionPortfolioEstate();

		Estate *m_estate;
		QPixmap *qpixmap, *icon;
		KPixmap *m_quartzBlocks;
		bool b_recreate, m_recreateQuartz;
		int m_orientation, m_titleWidth, m_titleHeight;
		QTextView *lname;
		PortfolioEstate *pe;

	private slots:
		void slotMenuAction(int);
		void estateChanged();
};

#endif
