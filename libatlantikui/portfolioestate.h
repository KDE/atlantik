#ifndef ATLANTIK_PORTFOLIOESTATE_H
#define ATLANTIK_PORTFOLIOESTATE_H

#include <qpixmap.h>
#include <qwidget.h>

#define PE_WIDTH	13
#define	PE_HEIGHT	16

class Estate;
class Player;
  
class PortfolioEstate : public QWidget
{
Q_OBJECT

public:
	PortfolioEstate(Estate *estate, Player *player, bool alwaysOwned, QWidget *parent, const char *name = 0);
	Estate *estate() { return m_estate; }
	static QPixmap drawPixmap(Estate *estate, Player *player = 0, bool alwaysOwned = true);

	protected:
		void paintEvent(QPaintEvent *);

	private:
		Estate *m_estate;
		Player *m_player;
		QPixmap m_pixmap;
		bool b_recreate, m_alwaysOwned;

private slots:
	void estateChanged();
};

#endif
