#ifndef ATLANTIK_ESTATE_H
#define ATLANTIK_ESTATE_H

#include "estateview.h"

class Player;

class Estate : public QObject
{
Q_OBJECT

public:
	Estate(int estateId);
	const int estateId() { return m_estateId; }
	void setName(const QString name);
	const QString name() { return m_name; }
	void setOwner(Player *player);
	void setHouses(unsigned int houses);
	unsigned int houses() { return m_houses; }
	void setCanBeOwned(const bool canBeOwned);
	const bool canBeOwned() { return m_canBeOwned; }
	void setIsMortgaged(const bool isMortgaged);
	const bool isMortgaged() { return m_isMortgaged; }
	void setCanToggleMortgage(const bool canToggleMortgage);
	const bool canToggleMortgage() { return m_canToggleMortgage; }
	void setBgColor(const QColor color);
	const QColor bgColor() { return m_bgColor; }
	void update();

signals:
	void changed();

private:
	bool m_changed;
	int m_estateId;
	QString m_name;
	Player *m_owner;
	unsigned int m_houses;
	bool m_canBeOwned, m_isMortgaged, m_canToggleMortgage;
	QColor m_bgColor, m_fgColor;
};

#endif
