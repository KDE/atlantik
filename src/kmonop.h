#ifndef __KMONOP_KMONOP_H__
#define __KMONOP_KMONOP_H__

#include <qwidget.h>
#include <qlayout.h>
#include <qtextview.h>

#include <ktmainwindow.h>
#include <kaction.h>

#include "configdlg.h"
#include "newgamedlg.h"
#include "network.h"
#include "portfolioview.h"
#include "board.h"

/**
 * Main KMonop window.
 * Manages gameboard, portfolios and pretty much everything else.
 *
 * @author Rob Kaper <cap@capsi.com>
 */
class KMonop : public KTMainWindow
{
Q_OBJECT
	
public:
	/**
	 * Create a KMonop window.
	 *
	 */
	KMonop(const char *name=0);

	/**
	 * Read the configuration settings using KConfig.
	 *
	 */
	void readConfig();
		
public slots:
	/**
	 * Creates a new modal dialog to open new games with.
	 *
	 */
	void slotNewGame();

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
	 * Writes the command to roll the dice to the network interface.
	 *
	 */
	void slotRoll();

	/**
	 * Writes the command to buy the current estate to the network interface.
	 *
	 */
	void slotBuy();

	/**
	 * Writes the command to end the turn to the network interface.
	 *
	 */
	void slotEndTurn();

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
	 * server to the text view as informational message.
	 *
	 * @param msg The informational message to be appended.
	 */
	void slotMsgInfo(QString msg);

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

	/**
	 * Updates the playername label in the appropriate player portfolio.
	 *
	 * @param playerid Player identifier.
	 * @param name     Player name.
	 */
	void slotMsgPlayerUpdateName(int playerid, QString name);

	/**
	 * Updates the money label in the appropriate player portfolio.
	 *
	 * @param playerid Player identifier.
	 * @param name     Amount of money.
	 */
	void slotMsgPlayerUpdateMoney(int, QString);

	/**
	 * Updates whether an estateview is owned in the appropriate player
	 * portfolio and calls the appropriate method for the gameboard
	 * member.
	 *
	 * @param estateId Estate identifier.
	 * @param playerId Player identifier. A value of -1 equals unowned.
	 */
	void slotMsgEstateUpdateOwner(int estateId, int playerId);

	/**
	 * Stores the playerid corresponding to this instance of the client.
	 *
	 * @param playerid Player identifier.
	 */
	void slotSetPlayerId(int playerid);

	/**
	 * Updates various visualisation parts of the player at turn. If
	 * playerid matches the stored playerid, toolbar buttons are
	 * enabled, otherwise disabled. Raises the appropriate token on the
	 * gameboard and visualises player at turn in the portfolioview.
	 *
	 * @param playerid Player identifier.
	 */
	void slotSetTurn(int playerid);
	
private:
	/**
	 * Private member, appends a message the text view.
	 *
	 * @param msg Message to be appended.
	 */
	void serverMsgsAppend(QString msg);

	QWidget *m_mainWidget;
	QLineEdit *m_input;
	QGridLayout *m_mainLayout;
	QTextView *m_serverMsgs, *m_chatMsgs;

	KAction *m_roll, *m_buyEstate, *m_configure, *m_endTurn;

	NewGameWizard *m_newgameWizard;
	ConfigDialog *m_configDialog;
	PortfolioView *m_portfolioArray[6];
	KMonopBoard *m_board;

	int m_myPlayerId;
};

#endif
