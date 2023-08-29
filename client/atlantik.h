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

#include <QWidget>
#include <qtextedit.h>
#include <QList>
#include <QAbstractSocket>
#include <QColor>
#include <QPointer>

#include <kxmlguiwindow.h>

#include <tokentheme.h>
#include <network_defs.h>

#include "connectioncookie.h"

#include <memory>

class QScrollArea;
class QAction;
class QLineEdit;
class QLabel;
class QVBoxLayout;
class QGridLayout;
class QCommandLineParser;
class AtlanticCore;
class AtlantikNetwork;
class PortfolioView;
class AtlantikBoard;
class Auction;
class ClickableLabel;

struct AtlantikConfig
{
	// General options;
	bool chatTimestamps;

	// Personalization options
	QString playerName, playerImage;

	// Board options
	bool indicateUnowned;
	bool highlightUnowned;
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
	LogTextEdit(QWidget *parent = nullptr);
	virtual ~LogTextEdit();

protected:
	void contextMenuEvent(QContextMenuEvent *event) override;

private:
	QAction *m_clear, *m_selectAll, *m_copy;
};

/**
 * Main Atlantik window.
 * Manages gameboard, portfolios and pretty much everything else.
 *
 * @author Rob Kaper <cap@capsi.com>
 */
class Atlantik : public KXmlGuiWindow
{
Q_OBJECT

public:
	/**
	 * Create an Atlantik window.
	 *
	 */
	Atlantik(QCommandLineParser *parser = nullptr);

	/**
	 * Read the configuration settings using KConfig.
	 *
	 */
	void readConfig();

	AtlantikConfig config() { return m_config; }

private Q_SLOTS:
	void showSelectServer();
	void showSelectGame();
	void showSelectConfiguration();
	void initBoard();
	void showBoard();
	void freezeBoard();
	void clientCookie(const QString &cookie);
	void sendHandshake();
	void showAboutDialog();

public Q_SLOTS:

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
	 * @param errno See http://doc.qt.io/qt-4.8/qabstractsocket.html#SocketError-enum
	 */
	void slotNetworkError(QAbstractSocket::SocketError errnum);

	void slotNetworkDisconnected();

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
	void slotMsgInfo(const QString &msg);

	void slotMsgStatus(const QString &message, EventType type = ET_Generic);

	/**
	 * Informs serverMsgs() to append an incoming message from the
	 * server to the text view as error message.
	 *
	 * @param msg The error message to be appended.
	 */
	void slotMsgError(const QString &msg);

	/**
	 * Informs serverMsgs() to append an incoming message from the
	 * server to the text view as chat message.
	 *
	 * @param player The name of the player chatting.
	 * @param msg    The chat message to be appended.
	 */
	void slotMsgChat(const QString &player, const QString &msg);

	void slotReconnect();
	void slotLeaveGame();

	void newPlayer(Player *player);
	void newEstate(Estate *estate);
	void newTrade(Trade *trade);
	void newAuction(Auction *auction);

	void removeGUI(Player *player);
	void removeGUI(Trade *trade);

	void playerChanged(Player *player);
	void gainedTurn();

Q_SIGNALS:
	void rollDice();
	void buyEstate();
	void auctionEstate();
	void endTurn();
	void jailCard();
	void jailPay();
	void jailRoll();
	void reconnect(const QString &cookie);

protected:
	void closeEvent(QCloseEvent *) override;

private:
	enum MsgType { ErrorMsg, InfoMsg, ChatMsg };
	void initNetworkObject();
	PortfolioView *addPortfolioView(Player *player);
	PortfolioView *findPortfolioView(Player *player);
	void appendMsg(const QString &msg, MsgType type);

	QCommandLineParser *m_cliParser;

	QScrollArea *m_portfolioScroll;
	QWidget *m_mainWidget, *m_portfolioWidget;
	QGridLayout *m_mainLayout;
	QVBoxLayout *m_portfolioLayout;
	ClickableLabel *m_sbVersion;
	ClickableLabel *m_sbStatus;

	QLabel *m_portfolioLabel;
	QLineEdit *m_input;
	QTextEdit *m_serverMsgs;

	QAction *m_roll, *m_buyEstate, *m_auctionEstate, *m_endTurn,
		*m_jailCard, *m_jailPay, *m_jailRoll, *m_configure,
		*m_showEventLog, *m_reconnect, *m_leaveGame;

	AtlanticCore *m_atlanticCore;
	AtlantikNetwork *m_atlantikNetwork;
	AtlantikConfig m_config;

	AtlantikBoard *m_board;
	SelectServer *m_selectServer;
	SelectGame *m_selectGame;
	SelectConfiguration *m_selectConfiguration;
	EventLog *m_eventLog;
	QPointer<EventLogWidget> m_eventLogWidget;

	QList<PortfolioView *> m_portfolioViews;
	QMap<Trade *, TradeDisplay *> m_tradeGUIMap;

	bool m_runningGame;

	std::unique_ptr<ConnectionCookie> m_cookie;
	std::unique_ptr<ConnectionCookie> m_reconnectCookie;
	bool m_reconnecting;

	TokenTheme m_tokenTheme;
};

#endif
