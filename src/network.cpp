#warning remove iostream output
#include <iostream.h>

#include "network.moc"

GameNetwork::GameNetwork(QObject *parent, const char *name) : QSocket(parent, name)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(slotRead()));
}

void GameNetwork::writeData(const char *input)
{
	QString str(input);
	str.append("\n");
	writeBlock(str.latin1(), strlen(str.latin1()));
}

void GameNetwork::slotRead()
{
	QString str;
	while(canReadLine())
	{
		str = readLine();
		processMsg(str);
	}
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
						else if (type=="add" || type=="full")
							emit gamelistAdd(e_game.attributeNode(QString("id")).value(), e_game.attributeNode(QString("players")).value());
					}
					n_game = n_game.nextSibling();
				}
				emit gamelistEndUpdate(type);
			}
			else if (e.tagName() == "joinedgame")
			{
				// code to set playerid so we know when it's our turn or not
				int playerid = e.attributeNode(QString("playerid")).value().toInt();
				emit setPlayerId(playerid);
			}
			else if (e.tagName() == "newturn")
			{
				// Find out which player has the turn now
				int player = e.attributeNode(QString("player")).value().toInt();
				emit setTurn(player);
			}
			else if (e.tagName() == "playerlist")
			{
				emit clearPlayerList();

				QDomNode n_player = n.firstChild();
				while(!n_player.isNull())
				{
					QDomElement e_player = n_player.toElement();
					if (!e_player.isNull() && e_player.tagName() == "player")
					{
						emit addToPlayerList(e_player.attributeNode(QString("name")).value(), e_player.attributeNode(QString("host")).value());
					}
					n_player = n_player.nextSibling();
				}
			}
			else if (e.tagName() == "playerupdate")
				emit msgPlayerUpdate(e);
			else if (e.tagName() == "estateupdate")
			{
				int id = e.attributeNode(QString("id")).value().toInt();
				int owner = e.attributeNode(QString("owner")).value().toInt();
				emit msgEstateUpdate(id, owner);
			}
		}
		QDomNode node = n.firstChild();
		processNode(node);
		n = n.nextSibling();
	}
}
