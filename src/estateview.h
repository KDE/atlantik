#ifndef __KMONOP_ESTATEVIEW_H__
#define __KMONOP_ESTATEVIEW_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qlabel.h>

#include "portfolioestate.h"

enum Orientation{ North=0, East=1, South=2, West=3 };

class EstateView : public QWidget
{
Q_OBJECT

	public:
		EstateView(int id, int orientation, bool canBeOwned, const QColor &, const QString &, QWidget *parent, const char *name = 0);
		QPixmap *initIcon(QString);
		void setName(const char *);
		void setHouses(int);
		void setMortgaged(bool);
		void setOwned(bool);

		bool mortgaged();
		bool owned();

		void updatePE();
		void updateMortgaged();

	public slots:
		void slotResizeAftermath();

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);
		void mousePressEvent(QMouseEvent *);

	private:
		void centerPortfolioEstate();

		QPixmap *qpixmap, *icon;
		bool b_recreate;
		bool m_owned, m_canBeOwned, m_mortgaged;
		int m_id, m_orientation, m_houses;
		QColor m_color;
		QLabel *lname;
		QString estatename;
		PortfolioEstate *pe;

	private slots:
		void slotMenuAction(int);
};

#endif
