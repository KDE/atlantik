#ifndef __BOARD_H__
#define __BOARD_H__

#include <qwidget.h>
#include <qpainter.h>
#include <iostream.h>

class KMonopBoard : public QWidget
{
Q_OBJECT

	public:
		KMonopBoard(QWidget *parent, const char *name=0);
//		int KMonopBoard::heightForWidth(int);
	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);
		QWidget *sp1, *sp2;
};

#endif
