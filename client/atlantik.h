// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef ATLANTIK_ATLANTIK_H
#define ATLANTIK_ATLANTIK_H

#include <qwidget.h>
#include <qlayout.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qptrlist.h>

#include <kmainwindow.h>
#include <kaction.h>

#include "configdlg.h"
#include "portfolioview.h"
#include "board.h"

class AtlanticCore;
class AtlantikNetwork;

struct AtlantikConfig
{
	// Personalization options
	QString playerName;

	// Board options
	bool indicateUnowned;
	bool highliteUnowned;
	bool darkenMortgaged;
	bool quartzEffects;
	bool animateTokens;

	// Portfolio colors
	QColor activeColor, inactiveColor;
};

class SelectServer;
class SelectGame;
class SelectConfiguration;
class TradeDisplay;

class Player;
class Estate;
class Trade;

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

public slots:
	/**
	 * A network connection has been established, so we can show the game
	 * list instead of the server list.
	 *
	 */
	 void slotNetworkConnected();

	/**
	 * An error occured while setting up the network connection. Inform the
	 * user.
	 *
	 * @param errno See http://doc.trolltech.com/3.0/qsocket.html#Error-enum
	 */
	 void slotNetworkError(int errnum);

	/**
	 * A game was succesfully created or joined, so we can show the
	 * configuration dialog instead of the game list.
	 *
	 */
	 void slotJoinedGame();

	/**
 	 * The game is starting, so we can show the game board instead of the
	 * pre-game configuration.
	 *
	 */
	 void initGame();
	 void gameStarted();

	/**
	 * Creates a new modeless configure dialog or raises it when it already exists.
	 *
	 */
	void slotConfigure();

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
	void createGUI(Trade *trade);
	void newAuction(Auction *auction);

	void removeGUI(Trade *trade);

	void playerChanged();

private:
	QWidget *m_mainWidget, *m_portfolioWidget;
	QGridLayout *m_mainLayout;
	QVBoxLayout *m_portfolioLayout;

	QLabel *m_portfolioLabel;
	QLineEdit *m_input;
	QTextEdit *m_serverMsgs, *m_chatMsgs;

	KAction *m_roll, *m_buyEstate, *m_auctionEstate, *m_endTurn,
		*m_jailCard, *m_jailPay, *m_jailRoll, *m_configure;

	AtlanticCore *m_atlanticCore;
	AtlantikNetwork *m_atlantikNetwork;
	AtlantikConfig m_config;

	Player *m_playerSelf;

	ConfigDialog *m_configDialog;
	AtlantikBoard *m_board;
	SelectServer *m_selectServer;
	SelectGame *m_selectGame;
	SelectConfiguration *m_selectConfiguration;

	QPtrList<PortfolioView> m_portfolioViews;
	QMap<Trade *, TradeDisplay *> m_tradeGUIMap;
};

#endif
