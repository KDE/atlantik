#ifndef ATLANTIK_TOKEN_H
#define ATLANTIK_TOKEN_H

#include <qwidget.h>
#include <qpixmap.h>

class Player;
class Estate;
class EstateView;
class AtlantikBoard;

class Token : public QWidget
{
Q_OBJECT

	public:
		Token (Player *player, AtlantikBoard *parent, const char *name = 0);
		void setLocation(EstateView *estateView);
		EstateView *location() { return m_location; }
		void setDestination(EstateView *estateView);
		EstateView *destination() { return m_destination; }

	private slots:
		void playerChanged();

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);

signals:
	void tokenConfirmation(Estate *);

private:
		void updateGeometry();

		Player *m_player;
		EstateView *m_location, *m_destination;
		AtlantikBoard *m_parentBoard;
		bool b_recreate;
		QPixmap *qpixmap;
};

#endif
