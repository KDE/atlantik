#ifndef ATLANTIK_PORTFOLIOESTATE_H
#define ATLANTIK_PORTFOLIOESTATE_H

#include <qwidget.h>
#include <qpixmap.h>

#define PE_WIDTH	13
#define	PE_HEIGHT	16
  
class PortfolioEstate : public QWidget
{
	public:
		PortfolioEstate(QWidget *parent, const char *name = 0);
		void setColor(const QColor &);
		void setOwned(bool);

	protected:
		void paintEvent(QPaintEvent *);

	private:
		QColor color;
		QPixmap *qpixmap;
		bool b_recreate;
		bool owned;
};

#endif
