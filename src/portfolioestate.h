#ifndef __KMONOP_PORTFOLIOESTATE_H__
#define __KMONOP_PORTFOLIOESTATE_H__

#include <qwidget.h>
#include <qpixmap.h>
  
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
