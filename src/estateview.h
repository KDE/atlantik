#ifndef ATLANTIK_ESTATEVIEW_H
#define ATLANTIK_ESTATEVIEW_H

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
		void setCanBeMortgaged(bool);
		void setCanBeUnmortgaged(bool);
		void setOwned(bool byAny, bool byThisClient);

		bool mortgaged();
		bool canBeMortgaged();
		bool canBeUnmortgaged();
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

		QPixmap *qpixmap, *icon;
		KPixmap *m_quartzBlocks;
		bool b_recreate;
		bool m_ownedByAny, m_ownedByThisClient, m_canBeOwned, m_mortgaged, m_canBeMortgaged, m_canBeUnmortgaged;
		int m_id, m_orientation, m_houses, m_titleWidth, m_titleHeight;
		QColor m_color;
		QLabel *lname;
		QString estatename;
		PortfolioEstate *pe;

	private slots:
		void slotMenuAction(int);
};

#endif
