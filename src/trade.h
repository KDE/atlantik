#ifndef ATLANTIK_TRADE_H
#define ATLANTIK_TRADE_H

#include <qstring.h>
#include <qptrlist.h>
#include <portfolioview.h>

class Player;
class Trade;
class TradeDisplay;
class GameNetwork;

class TradeItem
{
public:
	TradeItem(Trade *trade, Player *from, Player *to);
	virtual ~TradeItem() { }
	
	Player *from() { return mFrom; }
	Player *to() { return mTo; }
	void setTo(Player *p) { mTo=p; }
	Trade *trade() { return mTrade; }

	/**
	 * how to visualize this
	 **/
	virtual QString text() const=0;

	/**
	 * the command sent to the server when this is created
	 **/
	virtual void createCommand()=0;
	
	virtual void destroyCommand()=0;
	
private:
	Player *mFrom, *mTo;
	Trade *mTrade;

};

class TradeEstate : public TradeItem
{
public:
	TradeEstate(Estate *estate, Trade *trade, Player *to);

	Estate *estate() { return mEstate; }
	
	virtual QString text() const;
	virtual void createCommand();
	virtual void destroyCommand();

private:
	Estate *mEstate;
};

class TradeCash : public TradeItem
{
public:
	TradeCash(int cash, Trade *trade, Player *from, Player *to);

	int cash() const { return mCash; }
	void setCash(int cash);
	
	virtual QString text() const;
	virtual void createCommand();
	virtual void destroyCommand();

private:
	int mCash;
};


class Trade : public QObject
{
Q_OBJECT

public:
	Trade(GameNetwork *network, int tradeId);
	int tradeId() { return m_tradeId; }

	void addPlayer(Player *);
	
	QPtrList<Player> players() const;

public slots:
	/**
	 * make this take effect on the server
	 **/
	void addTradeItem(TradeItem *i);

private slots:	
	/**
	 * tell someone that this changed
	 **/
	void changed(TradeItem *i) { emit tradeChanged(i); }

public:
	void updateEstate(Estate *estate, Player *player);
	void updateMoney(Player *from, Player *to, unsigned int money);
	
signals:
	void playerAdded(Player *);
	void playerRemoved(Player *);

	void tradeAdded(TradeItem *);
	void tradeRemoved(TradeItem *);
	void tradeChanged(TradeItem *);
	
private:
	int m_tradeId;

	QPtrList<Player> mPlayers;
	TradeDisplay *mDisplay;
	
	QPtrList<TradeItem> mTradeItems;
};

#endif
