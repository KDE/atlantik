#ifndef __KMONOP_ESTATEVIEW_H__
#define __KMONOP_ESTATEVIEW_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qlabel.h>

#include <kpixmap.h>

#include "portfolioestate.h"

enum Orientation{ North=0, East=1, South=2, West=3 };

class EstateView : public QWidget
{
Q_OBJECT

	public:
		EstateView(int id, int orientation, bool canBeOwned, const QColor &, const QString &, QWidget *parent, const char *name = 0);
		void setName(const char *);
		void setHouses(int);
		void setMortgaged(bool);
		void setOwned(bool);

		bool mortgaged();
		bool owned();

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

		QPixmap *qpixmap, *icon;
		KPixmap *m_quartzBlocks;
		bool b_recreate;
		bool m_owned, m_canBeOwned, m_mortgaged;
		int m_id, m_orientation, m_houses, m_titleWidth, m_titleHeight;
		QColor m_color;
		QLabel *lname;
		QString estatename;
		PortfolioEstate *pe;

	private slots:
		void slotMenuAction(int);
};

#endif
