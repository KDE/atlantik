#ifndef __KMONOP_PORTFOLIOVIEW_H__
#define __KMONOP_PORTFOLIOVIEW_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qlabel.h>

#include "portfolioestate.h"

class PortfolioView : public QWidget
{
	public:
		PortfolioView(QWidget *parent, const char *name = 0);
		void setName(const char *);
		void setCash(const char *);
		void setOwned(int, bool);
		void setHasTurn(bool);

	protected:
		void paintEvent(QPaintEvent *);
	
	private:
		QPixmap *qpixmap;
		bool b_recreate;
		QLabel *lname, *lcash;
		PortfolioEstate *estate[40];
		bool myHasTurn;
};

#endif
