#include <qlayout.h>
#include <qpainter.h>
#include <iostream.h>

#include "board.moc"
#include "estateview.h"

extern QColor kmonop_dpurple, kmonop_lblue, kmonop_purple, kmonop_orange,
kmonop_red, kmonop_yellow, kmonop_green, kmonop_blue, kmonop_greenbg;

KMonopBoard::KMonopBoard(QWidget *parent, const char *name) : QWidget(parent, name)
{
	cout << "created kmonopboard" << endl;
	setMinimumWidth(160);
	setMinimumHeight(160);

	QGridLayout *layout = new QGridLayout(this, 25, 25);

	spacer = new QWidget(this);
	layout->addWidget(spacer, 24, 24); // SE

	EstateView *estate;
	int i=0;

	QColor color;
	for (i=0;i<10;i++)
	{
		switch(i)
		{
			case 1: case 3: case 4:
				color = kmonop_purple; break;
			case 6: case 8: case 9:
				color = kmonop_orange; break;
			default:
				color = NULL;
		}
		estate = new EstateView(East, color, this, "aap");
		if (i==0)
			layout->addMultiCellWidget(estate, 21, 23, 0, 2);
		else
			layout->addMultiCellWidget(estate, 21-(i*2), 23-1-(i*2), 0, 2);
//		estate->setMinimumWidth(h);
//		estate->setMinimumHeight(w);
	}
	for (i=0;i<10;i++)
	{
		switch(i)
		{
			case 1: case 2: case 4:
				color = kmonop_green; break;
			case 7: case 9:
				color = kmonop_blue; break;
			default:
				color = NULL;
		}
		estate = new EstateView(West, color, this, "aap");
		if (i==0)
			layout->addMultiCellWidget(estate, 0, 2, 21, 23);
		else
			layout->addMultiCellWidget(estate, (i*2)+1, (i*2)+2, 21, 23);
//		estate->setMinimumWidth(h);
//		estate->setMinimumHeight(w);
	}
	for (i=0;i<10;i++)
	{
		switch(i)
		{
			case 1: case 3: case 4:
				color = kmonop_red; break;
			case 6: case 7: case 9:
				color = kmonop_yellow; break;
			default:
				color = NULL;
		}
		estate = new EstateView(South, color, this, "aap");
		if (i==0)
			layout->addMultiCellWidget(estate, 0, 2, (i*2), 2+(i*2));
		else
			layout->addMultiCellWidget(estate, 0, 2, 1+(i*2), 2+(i*2));
//		estate->setMinimumWidth(w);
//		estate->setMinimumHeight(h);
	}
	for (i=0;i<10;i++)
	{
		switch(i)
		{
			case 1: case 3:
				color = kmonop_dpurple; break;
			case 6: case 8: case 9:
				color = kmonop_lblue; break;
			default:
				color = NULL;
		}
		estate = new EstateView(North, color, this, "aap");
		if (i==0)
			layout->addMultiCellWidget(estate, 21, 23, 21, 23);
		else
			layout->addMultiCellWidget(estate, 21, 23, 21-(i*2), 23-1-(i*2));
//		estate->setMinimumWidth(w);
//		estate->setMinimumHeight(h);
	}
}

void KMonopBoard::resizeEvent(QResizeEvent *e)
{
	int q = e->size().width() - e->size().height();
	if (q > 0)
	{
		QSize s(q, 0);
		spacer->setFixedSize(s);
	}
	else
	{
		QSize s(0, -q);
		spacer->setFixedSize(s);
	}
}
