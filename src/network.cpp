#warning remove iostream output
#include <iostream.h>

#include "network.moc"

GameNetwork *gameNetwork;

GameNetwork::GameNetwork(QObject *parent, const char *name) : QSocket(parent, name)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(slotRead()));
}

void GameNetwork::writeData(const char *input)
{
	QString str(input);
	str.append("\n");
	if (state()==QSocket::Connection)
	{
		cout << "out [" << str << "]" << endl;
		writeBlock(str.latin1(), strlen(str.latin1()));
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
						emit msgEstateUpdateMortgage(estateid, a.value().toInt());
				}
			}
		}
		QDomNode node = n.firstChild();
		processNode(node);
		n = n.nextSibling();
	}
}
