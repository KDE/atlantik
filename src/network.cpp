#warning remove iostream output
#include <iostream.h>

#include "network.moc"

GameNetwork *gameNetwork;

GameNetwork::GameNetwork(QObject *parent, const char *name) : QSocket(parent, name)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(slotRead()));
}

void GameNetwork::cmdRoll()
{	writeData(".r");
}

void GameNetwork::cmdBuyEstate()
{	writeData(".eb");
}

void GameNetwork::cmdGameStart()
{	writeData(".gs");
}

void GameNetwork::cmdEndTurn()
{	writeData(".E");
}

void GameNetwork::cmdName(QString name)
{
	QString msg(".n");
	msg.append(name);
	writeData(msg);
}

void GameNetwork::cmdTokenConfirmation(int estateId)
{
	QString msg(".t");
	msg.append(QString::number(estateId));
	writeData(msg);
}

void GameNetwork::cmdEstateMortgage(int estateId)
{
	QString msg(".m");
	msg.append(QString::number(estateId));
	writeData(msg);
}

void GameNetwork::cmdEstateUnmortgage(int estateId)
{
	QString msg(".u");
	msg.append(QString::number(estateId));
	writeData(msg);
}

void GameNetwork::cmdHouseBuy(int estateId)
{
	QString msg(".hb");
	msg.append(QString::number(estateId));
	writeData(msg);
}

void GameNetwork::cmdHouseSell(int estateId)
{
	QString msg(".hs");
	msg.append(QString::number(estateId));
	writeData(msg);
}

void GameNetwork::cmdGamesList()
{	writeData(".gl");
}

void GameNetwork::cmdGameNew()
{	writeData(".gn");
}

void GameNetwork::cmdGameJoin(int gameId)
{
	QString msg(".gj");
	msg.append(QString::number(gameId));
	writeData(msg);
}

void GameNetwork::cmdChat(QString msg)
{	writeData(msg);
}

void GameNetwork::cmdTradeNew()
{	writeData(".Tn");
}

void GameNetwork::cmdTradeToggleEstate(int tradeId, int estateId)
{
	QString msg(".Te");
	msg.append(QString::number(estateId));
	msg.append(":");
	msg.append(QString::number(tradeId));
	writeData(msg);
}

void GameNetwork::cmdTradeSetMoney(int tradeId, int amount)
{
	QString msg(".Tm");
	msg.append(QString::number(amount));
	msg.append(":");
	msg.append(QString::number(tradeId));
	writeData(msg);
}

void GameNetwork::cmdTradeAccept(int tradeId)
{
	QString msg(".Ta");
	msg.append(QString::number(tradeId));
	writeData(msg);
}

void GameNetwork::cmdTradeReject(int tradeId)
{
	QString msg(".Tr");
	msg.append(QString::number(tradeId));
	writeData(msg);
}

void GameNetwork::writeData(QString msg)
{
	msg.append("\n");
	if (state()==QSocket::Connection)
	{
		cout << "out [" << msg << "]" << endl;
		writeBlock(msg.latin1(), strlen(msg.latin1()));
	}
}

void GameNetwork::slotRead()
{
	while(canReadLine())
		processMsg(readLine());

	if (bytesAvailable() > (1024 * 32))
		flush();
}

void GameNetwork::processMsg(QString str)
{
	cout << "processing msg: " + str << endl;
	msg.setContent(str);
	QDomElement e = msg.documentElement();
	if (e.tagName() != "monopd")
	{
		cout << "invalid message: " << str << endl;
		return;
	}
	QDomNode n = e.firstChild();
	processNode(n);
}

void GameNetwork::processNode(QDomNode n)
{
	QDomAttr a;

	while(!n.isNull())
	{
		QDomElement e = n.toElement();
		if(!e.isNull())
		{
			if (e.tagName() == "msg")
			{
				a = e.attributeNode(QString("type"));
				if (!a.isNull())
				{
					if (a.value() == "error")
						emit msgError(e.attributeNode(QString("value")).value());
					else if (a.value() == "info")
						emit msgInfo(e.attributeNode(QString("value")).value());
					else if (a.value() == "chat")
						emit msgChat(e.attributeNode(QString("author")).value(), e.attributeNode(QString("value")).value());
					else if (a.value() == "startgame")
						emit msgStartGame(e.attributeNode(QString("value")).value());
				}
			}
			else if (e.tagName() == "updategamelist")
			{
				QString type = e.attributeNode(QString("type")).value();

				emit gamelistUpdate(type);

				QDomNode n_game = n.firstChild();
				while(!n_game.isNull())
				{
					QDomElement e_game = n_game.toElement();
					if (!e_game.isNull() && e_game.tagName() == "game")
					{
						if (type=="del")
							emit gamelistDel(e_game.attributeNode(QString("id")).value());
						else if (type=="edit")
							emit gamelistEdit(e_game.attributeNode(QString("id")).value(), e_game.attributeNode(QString("players")).value());
						else if (type=="add" || type=="full")
							emit gamelistAdd(e_game.attributeNode(QString("id")).value(), e_game.attributeNode(QString("players")).value());
					}
					n_game = n_game.nextSibling();
				}
				emit gamelistEndUpdate(type);
			}
			else if (e.tagName() == "updateplayerlist")
			{
				QString type = e.attributeNode(QString("type")).value();

				emit playerlistUpdate(type);

				QDomNode n_player = n.firstChild();
				while(!n_player.isNull())
				{
					QDomElement e_player = n_player.toElement();
					if (!e_player.isNull() && e_player.tagName() == "player")
					{
						if (type=="del")
							emit playerlistDel(e_player.attributeNode(QString("clientid")).value());
						else if (type=="edit")
							emit playerlistEdit(e_player.attributeNode(QString("clientid")).value(), e_player.attributeNode(QString("name")).value(), e_player.attributeNode(QString("host")).value());
						else if (type=="add" || type=="full")
							emit playerlistAdd(e_player.attributeNode(QString("clientid")).value(), e_player.attributeNode(QString("name")).value(), e_player.attributeNode(QString("host")).value());
					}
					n_player = n_player.nextSibling();
				}
				emit playerlistEndUpdate(type);
			}
			else if (e.tagName() == "client")
			{
				a = e.attributeNode(QString("playerid"));
				if (!a.isNull())
					emit setPlayerId(a.value().toInt());
			}
			else if (e.tagName() == "newturn")
			{
				// Find out which player has the turn now
				int player = e.attributeNode(QString("player")).value().toInt();
				emit setTurn(player);
			}
			else if (e.tagName() == "playerupdate")
			{
				int playerid = -1, location = -1;
				bool directmove = false;

				a = e.attributeNode(QString("playerid"));
				if (!a.isNull())
				{
					playerid = a.value().toInt();

					a = e.attributeNode(QString("name"));
					if (!a.isNull())
						emit msgPlayerUpdateName(playerid, a.value());

					a = e.attributeNode(QString("money"));
					if (!a.isNull())
						emit msgPlayerUpdateMoney(playerid, a.value());

					a = e.attributeNode(QString("location"));
					if (!a.isNull())
						location = a.value().toInt();

					if (location != -1)
					{
						a = e.attributeNode(QString("directmove"));
						if (!a.isNull())
							directmove = a.value().toInt();

						emit msgPlayerUpdateLocation(playerid, location, directmove);
					}
				}
			}
			else if (e.tagName() == "estateupdate")
			{
				int estateid = -1;

				a = e.attributeNode(QString("estateid"));
				if (!a.isNull())
				{
					estateid = a.value().toInt();

					a = e.attributeNode(QString("name"));
					if (!a.isNull())
						emit msgEstateUpdateName(estateid, a.value());

					a = e.attributeNode(QString("owner"));
					if (!a.isNull())
						emit msgEstateUpdateOwner(estateid, a.value().toInt());

					a = e.attributeNode(QString("houses"));
					if (!a.isNull())
						emit msgEstateUpdateHouses(estateid, a.value().toInt());

					a = e.attributeNode(QString("mortgaged"));
					if (!a.isNull())
						emit msgEstateUpdateMortgaged(estateid, a.value().toInt());

					a = e.attributeNode(QString("can_be_mortgaged"));
					if (!a.isNull())
						emit msgEstateUpdateCanBeMortgaged(estateid, a.value().toInt());

					a = e.attributeNode(QString("can_be_unmortgaged"));
					if (!a.isNull())
						emit msgEstateUpdateCanBeUnmortgaged(estateid, a.value().toInt());
				}
			}
			else if (e.tagName() == "tradeupdate")
			{
				QString type = e.attributeNode(QString("type")).value();
				int tradeid = e.attributeNode(QString("tradeid")).value().toInt();

				if (type=="new")
				{
					emit msgTradeUpdateNew(tradeid, e.attributeNode(QString("actor")).value().toInt());

					QDomNode n_player = n.firstChild();
					while(!n_player.isNull())
					{
						QDomElement e_player = n_player.toElement();
						if (!e_player.isNull() && e_player.tagName() == "player")
							emit msgTradeUpdatePlayerAdd(tradeid, e_player.attributeNode(QString("playerid")).value().toInt());
						n_player = n_player.nextSibling();
					}
				}
				else if (type=="edit")
				{
					QDomNode n_child = n.firstChild();
					while(!n_child.isNull())
					{
						QDomElement e_child = n_child.toElement();
						if (!e_child.isNull())
						{
							if (e_child.tagName() == "player")
							{
								a = e.attributeNode(QString("playerid"));
								if (!a.isNull())
								{
									int playerid = a.value().toInt();

									a = e.attributeNode(QString("accept"));
									if (!a.isNull())
										emit msgTradeUpdatePlayerAccept(tradeid, playerid, (bool)(a.value().toInt()));

									a = e.attributeNode(QString("money"));
									if (!a.isNull())
										emit msgTradeUpdatePlayerMoney(tradeid, playerid, a.value().toInt());
								}
							}
							else if (e_child.tagName() == "estate")
							{
								a = e.attributeNode(QString("estateid"));
								if (!a.isNull())
								{
									int estateid = a.value().toInt();

									a = e.attributeNode(QString("included"));
									if (!a.isNull())
										emit msgTradeUpdateEstateIncluded(tradeid, estateid, (bool)(a.value().toInt()));
								}
							}
						}
						n_child = n_child.nextSibling();
					}
				}
				else if (type=="accepted")
					emit msgTradeUpdateAccepted(tradeid);
				else if (type=="completed")
					emit msgTradeUpdateCompleted(tradeid);
				else if (type=="rejected")
					emit msgTradeUpdateRejected(tradeid, e.attributeNode(QString("actor")).value().toInt());
			}
		}
		QDomNode node = n.firstChild();
		processNode(node);
		n = n.nextSibling();
	}
}
