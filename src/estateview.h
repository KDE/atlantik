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
		EstateView(int _orientation, const QColor &, const QString &, QWidget *parent, const char *name = 0);
		QPixmap *initIcon(QString);
		void setName(const char *);
		void setHouses(int);
		void setOwned(bool);

	public slots:
		void slotResizeAftermath();

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);

	private:
		QPixmap *qpixmap, *icon;
		bool b_recreate;
		QColor color;		
		QLabel *lname;
		QString estatename;
		int orientation;
		int houses;
		PortfolioEstate *pe;
};

#endif
