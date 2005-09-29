// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef ATLANTIK_ATLANTIK_H
#define ATLANTIK_ATLANTIK_H

#include <qwidget.h>
#include <qlayout.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qptrlist.h>

#include <kmainwindow.h>

#include "configdlg.h"
#include "portfolioview.h"
#include "board.h"

class QScrollView;

class AtlanticCore;
class AtlantikNetwork;

struct AtlantikConfig
{
	// General options;
	bool chatTimestamps;

	// Personalization options
	QString playerName, playerImage;

	// Board options
	bool indicateUnowned;
	bool highliteUnowned;
	bool darkenMortgaged;
	bool quartzEffects;
	bool animateTokens;

	// Meta server options
	bool connectOnStart;
	bool hideDevelopmentServers;

	// Portfolio colors
	QColor activeColor, inactiveColor;
};

class EventLog;
class EventLogWidget;
class SelectServer;
class SelectGame;
class SelectConfiguration;
class TradeDisplay;

class Player;
class Estate;
class Trade;

class LogTextEdit : public QTextEdit
{
Q_OBJECT

public:
	LogTextEdit( QWidget *parent = 0, const char *name = 0 );
	virtual ~LogTextEdit();

	QPopupMenu *createPopupMenu( const QPoint & pos );

private:
	KAction *m_clear, *m_selectAll, *m_copy;
};

/**
 * Main Atlantik window.
 * Manages gameboard, portfolios and pretty much everything else.
 *
 * @author Rob Kaper <cap@capsi.com>
 */
class Atlantik : public KMainWindow
{
Q_OBJECT
	
public:
	/**
	 * Create an Atlantik window.
	 *
	 */
	Atlantik();

	/**
	 * Read the configuration settings using KConfig.
	 *
	 */
	void readConfig();

	/**
	 * Appends a message the text view.
	 *
	 * @param msg Message to be appended.
	 */
	void serverMsgsAppend(QString msg);

	AtlantikConfig config() { return m_config; }

private slots:
	void showSelectServer();
	void showSelectGame();
	void showSelectConfiguration();
	void initBoard();
	void showBoard();
	void freezeBoard();
	void clientCookie(QString cookie);
	void sendHandshake();
	void statusBarClick(int);

public slots:

	/**
	 * A network connection has been established, so we can show the game
	 * list instead of the server list.
	 *
	 */
	void slotNetworkConnected();

	/**
	 * An error occurred while setting up the network connection. Inform the
	 * user.
	 *
	 * @param errno See http://doc.trolltech.com/3.0/qsocket.html#Error-enum
	 */
	void slotNetworkError(int errnum);

	void networkClosed(int status);
	 
	/**
	 * Creates a new modeless configure dialog or raises it when it already exists.
	 *
	 */
	void slotConfigure();

	/**
	 * Opens the event log widget.
	 *
	 */
	void showEventLog();

	/**
	 * Opens the KNotify dialog for configuration events.
	 *
	 */
	void configureNotifications();

	/**
	 * Reads values from configuration dialog and stores them into
	 * global configuration struct. If values have changed, appropriate
	 * methods within the application are called. Configuration is saved
	 * to file in any case.
	 *
	 */
	void slotUpdateConfig();

	/**
	 * Writes the contents of the text input field to the network
	 * interface and clears the text input field.
	 *
	 */
	void slotSendMsg();

	/**
	 * Informs serverMsgs() to append an incoming message from the
	 * server to the text view as informational message.
	 *
	 * @param msg The message to be appended.
	 */
	void slotMsgInfo(QString msg);

	void slotMsgStatus(const QString &message, const QString &icon = QString::null);

	/**
	 * Informs serverMsgs() to append an incoming message from the
	 * server to the text view as error message.
	 *
	 * @param msg The error message to be appended.
	 */
	void slotMsgError(QString msg);

	/**
	 * Informs serverMsgs() to append an incoming message from the
	 * server to the text view as chat message.
	 *
	 * @param player The name of the player chatting.
	 * @param msg    The chat message to be appended.
	 */
	void slotMsgChat(QString player, QString msg);

	void newPlayer(Player *player);
	void newEstate(Estate *estate);
	void newTrade(Trade *trade);
	void newAuction(Auction *auction);

	void removeGUI(Player *player);
	void removeGUI(Trade *trade);

	void playerChanged(Player *player);
	void gainedTurn();

signals:
	void rollDice();
	void buyEstate();
	void auctionEstate();
	void endTurn();
	void jailCard();
	void jailPay();
	void jailRoll();

protected:
	void closeEvent(QCloseEvent *);

private:
	void initEventLog();
	void initNetworkObject();
	PortfolioView *addPortfolioView(Player *player);
	PortfolioView *findPortfolioView(Player *player);

	QScrollView *m_portfolioScroll;
	QWidget *m_mainWidget, *m_portfolioWidget;
	QGridLayout *m_mainLayout;
	QVBoxLayout *m_portfolioLayout;

	QLabel *m_portfolioLabel;
	QLineEdit *m_input;
	QTextEdit *m_serverMsgs;

	KAction *m_roll, *m_buyEstate, *m_auctionEstate, *m_endTurn,
		*m_jailCard, *m_jailPay, *m_jailRoll, *m_configure,
		*m_showEventLog;

	AtlanticCore *m_atlanticCore;
	AtlantikNetwork *m_atlantikNetwork;
	AtlantikConfig m_config;

	ConfigDialog *m_configDialog;
	AtlantikBoard *m_board;
	SelectServer *m_selectServer;
	SelectGame *m_selectGame;
	SelectConfiguration *m_selectConfiguration;
	EventLog *m_eventLog;
	EventLogWidget *m_eventLogWidget;

	QPtrList<PortfolioView> m_portfolioViews;
	QMap<Trade *, TradeDisplay *> m_tradeGUIMap;

	bool m_runningGame;
};

#endif
