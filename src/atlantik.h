#ifndef ATLANTIK_ATLANTIK_H
#define ATLANTIK_ATLANTIK_H

#include <qwidget.h>
#include <qlayout.h>
#include <qtextedit.h>
#include <qlabel.h>

#include <kmainwindow.h>
#include <kaction.h>

#include "configdlg.h"
#include "network.h"
#include "portfolioview.h"
#include "board.h"
#include "player.h"
#include "estate.h"
#include "trade.h"

class SelectServer;
class SelectGame;
class SelectConfiguration;

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
	 void slotNetworkError(int errno);

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
	 void slotInitGame();

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

	/**
	 * Hides the new game dialog and informs serverMsgs() to append an
	 * incoming message from the server to the text view at the start of
	 * the game.
	 *
	 * @param msg    The message to be appended.
	 */
	void slotMsgStartGame(QString msg);

	void slotMsgPlayerUpdateLocation(int playerId, int estateId, bool);

	/**
	 * Updates the playername label in the appropriate player portfolio.
	 *
	 * @param playerId Player identifier.
	 * @param name     Player name.
	 */
	void slotMsgPlayerUpdateName(int playerId, QString name);

	void slotMsgPlayerUpdateJailed(int playerId, bool inJail);

	/**
	 * Updates the money label in the appropriate player portfolio.
	 *
	 * @param playerId Player identifier.
	 * @param name     Amount of money.
	 */
	void slotMsgPlayerUpdateMoney(int playerId, QString money);

	/**
	 * Updates whether an estateview is owned in the appropriate player
	 * portfolio and calls the appropriate method for the gameboard
	 * member.
	 *
	 * @param estateId Estate identifier.
	 * @param playerId Player identifier. A value of -1 equals unowned.
	 */
	void slotMsgEstateUpdateOwner(int estateId, int playerId);

	void slotMsgEstateUpdateName(int, QString);
	void slotMsgEstateUpdateColor(int, QString);
	void slotMsgEstateUpdateBgColor(int, QString);
	void slotMsgEstateUpdateHouses(int, int);
	void slotMsgEstateUpdateMortgaged(int, bool);
	void slotMsgEstateUpdateCanToggleMortgage(int, bool);

	/**
	 * Tells estate object whether it can be owned or not.
	 *
	 * @param estateId Estate identifier.
	 * @param canBeOwned Boolean.
	 */
	void slotMsgEstateUpdateCanBeOwned(int estateId, bool canBeOwned);

	void slotEstateUpdateCanBuyHouses(int estateId, bool canBuyHouses);
	void slotEstateUpdateCanSellHouses(int estateId, bool canSellHouses);

	void slotEstateUpdateFinished(int estateId);
	void slotPlayerUpdateFinished(int playerId);

	void slotTradeUpdatePlayerAdd(int tradeId, int playerId);

	/**
	 * Stores the playerId corresponding to this instance of the client.
	 *
	 * @param playerId Player identifier.
	 */
	void slotSetPlayerId(int playerId);

	/**
	 * Updates various visualisation parts of the player at turn. If
	 * playerId matches the stored playerId, toolbar buttons are
	 * enabled, otherwise disabled. Raises the appropriate token on the
	 * gameboard and visualises player at turn in the portfolioview.
	 *
	 * @param playerId Player identifier.
	 */
	void slotSetTurn(int playerId);

	/**
	 * A new player object and view must be initialized.
	 *
	 * @param playerId Playerid as used by the server daemon.
	 */
	 void slotPlayerInit(int playerId);

	/**
	 * A new estate object and view must be initialized.
	 *
	 * @param estateId Estateid as used by the server daemon.
	 */
	 void slotEstateInit(int estateId);

	void slotTradeInit(int tradeid);

private:
	QWidget *m_mainWidget, *m_portfolioWidget;
	QGridLayout *m_mainLayout;
	QVBoxLayout *m_portfolioLayout;

	QLabel *m_portfolioLabel;
	QLineEdit *m_input;
	QTextEdit *m_serverMsgs, *m_chatMsgs;

	KAction *m_roll, *m_buyEstate, *m_endTurn,
		*m_jailCard, *m_jailPay, *m_jailRoll,
		*m_configure;

	GameNetwork *m_gameNetwork;

	ConfigDialog *m_configDialog;
	AtlantikBoard *m_board;
	SelectServer *m_selectServer;
	SelectGame *m_selectGame;
	SelectConfiguration *m_selectConfiguration;

	QMap<int, Player *> playerMap;
	QMap<int, Estate *> estateMap;
	QMap<int, Trade *> tradeMap;
	QMap<int, PortfolioView *> portfolioMap;
};

#endif
