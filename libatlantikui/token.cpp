#include <qpainter.h>
#include <qfont.h>

#include <kdebug.h>

#include <kstandarddirs.h>

#include "token.moc"
#include "player.h"
#include "board.h"
#include "estateview.h"

Token::Token(Player *player, AtlantikBoard *parent, const char *name) : QWidget(parent, name)
{
	setBackgroundMode(NoBackground); // avoid flickering

	m_player = player;

	m_parentBoard = parent;
	connect(this, SIGNAL(tokenConfirmation(Estate *)), m_parentBoard, SIGNAL(tokenConfirmation(Estate *)));

	qpixmap = 0;
	b_recreate = true;

	setFixedSize(QSize(26, 26));
	m_location = m_destination = 0;
}

void Token::setLocation(EstateView *estateView)
{
	if (m_location != estateView)
	{
		m_location = estateView;
		updateGeometry();
	}
}

void Token::setDestination(EstateView *estateView)
{
	if (m_destination != estateView)
	{
		m_destination = estateView;
		updateGeometry();
	}
}

void Token::playerChanged()
{
	if (m_player->hasTurn())
		raise();

	if (Estate *estate = m_player->location())
	{
		EstateView *estateView;
		QPtrList<EstateView> estateViews = m_parentBoard->estateViews();
		for (QPtrListIterator<EstateView> it(estateViews); *it; ++it)
		{
			if ((estateView = dynamic_cast<EstateView*>(*it)))
			{
				if (estateView->estate() == estate)
				{
					setLocation(estateView);
					break;
				}
			}
		}
	}
}

void Token::updateGeometry()
{
	if (m_location)
	{
		int x = m_location->geometry().center().x() - (width()/2);
		int y = m_location->geometry().center().y() - (height()/2);
		setGeometry(x, y, width(), height());
		show();
		emit tokenConfirmation(m_player->location());
	}
	else
	{
		hide();
	}
}

void Token::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(locate("data", "atlantik/pics/token.png"));

		QPainter painter;
		painter.begin(qpixmap, this);

		painter.drawPixmap(0, 0, *qpixmap);

		painter.setPen(Qt::black);
		painter.setFont(QFont("Helvetica", 12, QFont::Bold));
		painter.drawText(2, height()-2, m_player->name());

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void Token::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
}
