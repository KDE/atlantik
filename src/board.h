#ifndef __KMONOP_BOARD_H__
#define __KMONOP_BOARD_H__

#include <qwidget.h>

class KMonopBoard : public QWidget
{
Q_OBJECT

	public:
		KMonopBoard(QWidget *parent, const char *name=0);
	protected:
		void resizeEvent(QResizeEvent *);
		QWidget *spacer;
};

#endif
