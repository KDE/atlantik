#ifndef ATLANTIK_PLAYER_H
#define ATLANTIK_PLAYER_H

#include <qobject.h>
#include <qstring.h>

class Player : public QObject
{
Q_OBJECT

public:
	Player(int playerId);
	int playerId() { return m_playerId; }

	void setLocation(const int estateId);
	int location() const { return m_location; }
	void setIsSelf(const bool isSelf) { m_isSelf = isSelf; }
	bool isSelf() const { return m_isSelf; }
	void setHasTurn(const bool hasTurn);
	bool hasTurn() const { return m_hasTurn; }
	void setInJail(const bool inJail);
	bool inJail() const { return m_inJail; }
	void setName(const QString _n);
	QString name() const { return m_name; }
	void setMoney(unsigned int _m);
	unsigned int money() const { return m_money; }
	void update(bool force = false);

signals:
	void changed();

private:
	bool m_changed;
	int m_playerId, m_location;
	unsigned int m_money;
	bool m_isSelf, m_hasTurn, m_inJail;
	QString m_name;
};

#endif
