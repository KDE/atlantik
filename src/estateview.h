#ifndef ATLANTIK_ESTATEVIEW_H
#define ATLANTIK_ESTATEVIEW_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qlabel.h>

#include <kpixmap.h>

#include "portfolioestate.h"

enum Orientation{ North=0, East=1, South=2, West=3 };

class Estate;

class EstateView : public QWidget
{
Q_OBJECT

	public:
		EstateView(Estate *parentEstate, int orientation, const QColor &, const QString &, QWidget *parent, const char *name = 0);
		void setHouses(int);
		void setOwned(bool byAny, bool byThisClient);

		bool ownedByAny();
		bool ownedByThisClient();

		void updatePE();
		void redraw();

	public slots:
		void slotResizeAftermath();

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);
		void mousePressEvent(QMouseEvent *);

	private:
		QPixmap *rotatePixmap(QPixmap *);
		KPixmap *rotatePixmap(KPixmap *);
		void drawQuartzBlocks(KPixmap *pi, KPixmap &p, const QColor &c1, const QColor &c2);
		void repositionPortfolioEstate();

		Estate *m_parentEstate;
		QPixmap *qpixmap, *icon;
		KPixmap *m_quartzBlocks;
		bool b_recreate;
		bool m_ownedByAny, m_ownedByThisClient;
		int m_id, m_orientation, m_houses, m_titleWidth, m_titleHeight;
		QColor m_color;
		QLabel *lname;
		PortfolioEstate *pe;

	private slots:
		void slotMenuAction(int);
};

#endif
