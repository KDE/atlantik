#include <qlayout.h>
#include <qpainter.h>

#include "board.moc"
#include "estateview.h"

extern QColor kmonop_dpurple, kmonop_lblue, kmonop_purple, kmonop_orange,
kmonop_red, kmonop_yellow, kmonop_green, kmonop_blue, kmonop_greenbg;

KMonopBoard::KMonopBoard(GameNetwork *_nw, QWidget *parent, const char *name) : QWidget(parent, name)
{
	netw = _nw;

	setMinimumWidth(160);
	setMinimumHeight(160);

	QGridLayout *layout = new QGridLayout(this, 25, 25);

	spacer = new QWidget(this);
	layout->addWidget(spacer, 24, 24); // SE

	int i=0, orientation=North;

	QColor color;
	for (i=0;i<40;i++)
	{
		switch(i)
		{
			case 1: case 3:
				color = kmonop_dpurple; break;
			case 6: case 8: case 9:
				color = kmonop_lblue; break;
			case 11: case 13: case 14:
				color = kmonop_purple; break;
			case 16: case 18: case 19:
				color = kmonop_orange; break;
			case 21: case 23: case 24:
				color = kmonop_red; break;
			case 26: case 27: case 29:
				color = kmonop_yellow; break;
			case 31: case 32: case 34:
				color = kmonop_green; break;
			case 37: case 39:
				color = kmonop_blue; break;
			default:
				color = QColor();
		}

		if (i<10)
			orientation = North;
		else if (i<20)
			orientation = East;
		else if (i<30)
			orientation = South;
		else if (i<40)
			orientation = West;
			
		estate[i] = new EstateView(orientation, color, this, "estate");

		if (i==0)
			layout->addMultiCellWidget(estate[i], 21, 23, 21, 23);
		else if (i<10)
			layout->addMultiCellWidget(estate[i], 21, 23, 21-(i*2), 23-1-(i*2));
		else if (i==10)
			layout->addMultiCellWidget(estate[i], 21, 23, 0, 2);
		else if (i<20)
			layout->addMultiCellWidget(estate[i], 21-((i-10)*2), 23-1-((i-10)*2), 0, 2);
		else if (i==20)
			layout->addMultiCellWidget(estate[i], 0, 2, ((i-20)*2), 2+((i-20)*2));
		else if (i<30)
			layout->addMultiCellWidget(estate[i], 0, 2, 1+((i-20)*2), 2+((i-20)*2));
		else if (i==30)
			layout->addMultiCellWidget(estate[i], 0, 2, 21, 23);
		else
			layout->addMultiCellWidget(estate[i], ((i-30)*2)+1, ((i-30)*2)+2, 21, 23);

//		estate[i]->setMinimumWidth(w);
//		estate[i]->setMinimumHeight(h);
	}

	connect(netw, SIGNAL(msgPlayerUpdate(QDomNode)), this, SLOT(slotMsgPlayerUpdate(QDomNode)));

	for(i=0;i<MAXPLAYERS;i++)
		token[i]=0;
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

void KMonopBoard::slotMsgPlayerUpdate(QDomNode n)
{
	QDomAttr a_id, a_location;
	QDomElement e;
	int id=0;
	
	e = n.toElement();
	if(!e.isNull())
	{
		a_id = e.attributeNode(QString("id"));
		if (a_id.isNull())
			return;

		// Maximum of six players, right? Check with monopd, possibly fetch
		// number first and make portfolio overviews part of QHLayout to
		// allow any theoretically number.
		id = a_id.value().toInt() - 1;
		if (id < MAXPLAYERS)
		{
			if(token[id]==0)
			{
				token[id] = new Token(a_id.value(), estate[0], "token");
				token[id]->setGeometry(id*3, id*3, token[id]->width(), token[id]->height());
				token[id]->show();
			}

			a_location = e.attributeNode(QString("location"));
			if (!a_location.isNull())
			{
				delete token[id];
				token[id] = new Token(a_id.value(), estate[a_location.value().toInt()], "token");
				token[id]->setGeometry(id*3, id*3, token[id]->width(), token[id]->height());
				token[id]->show();
			}
		}
	}
}
