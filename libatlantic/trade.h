#ifndef LIBATLANTIC_TRADE_H
#define LIBATLANTIC_TRADE_H

#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h>

class Player;
class Trade;
class Estate;

class TradeItem : public QObject
{
Q_OBJECT

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
Q_OBJECT

public:
	TradeEstate(Estate *estate, Trade *trade, Player *to);

	Estate *estate() { return mEstate; }
	
	virtual QString text() const;
	virtual void createCommand();
	virtual void destroyCommand();

signals:
	void updateEstate(Trade *trade, Estate *estate, Player *player);
	void updateMoney(Trade *trade, unsigned int money, Player *from, Player *to);

private:
	Estate *mEstate;
};

class TradeMoney : public TradeItem
{
public:
	TradeMoney(unsigned int money, Trade *trade, Player *from, Player *to);

	unsigned int money() const { return mMoney; }
	void setMoney(unsigned int money) { mMoney = money; }
	
	virtual QString text() const;
	virtual void createCommand();
	virtual void destroyCommand();

private:
	unsigned int mMoney;
};


class Trade : public QObject
{
Q_OBJECT

public:
	Trade(int tradeId);
	int tradeId() { return m_tradeId; }

	void addPlayer(Player *);
	
	QPtrList<Player> players() const;
	
	/**
	 * select the Trade of the given template type, with the given
	 * from and to
	 **/
	template <class Type> Type *select(Player *from, Player *to)
	{
		for (QPtrListIterator<Player> i(mPlayers()); *i; ++i)
		{
			if (
					(*i)->from()==from
					&& (*i)->to()==to
					&& dynamic_cast<Player*>(*i)
				)
			{
				return *i;
			}
		}
	
	}

public slots:
	/**
	 * make this take effect on the server
	 **/
	void addTradeItem(TradeItem *i);
	/**
	 * call this to destroy a trade item and take it
	 * of the trade.  Do not delete it yourself
	 **/
	void removeTradeItem(TradeItem *i);

private slots:	
	/**
	 * tell someone that this changed
	 **/
	void changed(TradeItem *i) { emit tradeChanged(i); }

public:
	void update(bool force = false);
	void updateEstate(Estate *estate, Player *player);
	void updateMoney(unsigned int money, Player *from, Player *to);
	
signals:
	void changed();

	void playerAdded(Player *);
	void playerRemoved(Player *);

	void tradeAdded(TradeItem *);
	void tradeRemoved(TradeItem *);
	void tradeChanged(TradeItem *);

	void updateEstate(Trade *trade, Estate *estate, Player *to);
	void updateMoney(Trade *trade, unsigned int money, Player *from, Player *to);
	void reject(Trade *trade);

private:
	bool m_changed;
	int m_tradeId;

	QPtrList<Player> mPlayers;
	
	QPtrList<TradeItem> mTradeItems;
};

#endif
