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
		cout << "processing node: " << n.nodeName() << endl;
		QDomElement e = n.toElement();
		if(!e.isNull())
		{
			cout << "node is element: " << e.tagName() << endl;
			if (e.tagName() == "msg")
			{
				cout << "found message  ! " << endl;
				a = e.attributeNode(QString("type"));
				if (!a.isNull())
				{
					if (a.value() == "error")
					{
						cout << "MONOPD_ERROR    : " << e.attributeNode(QString("value")).value() << endl;
						emit msgError(e.attributeNode(QString("value")).value());
					}
					if (a.value() == "startgame")
					{
						cout << "MONOPD_GAMESTART: " << e.attributeNode(QString("value")).value() << endl;
						emit msgStartGame(e.attributeNode(QString("value")).value());
					}
				}
			}
			if (e.tagName() == "gamelist")
			{
				cout << "found gamelist ! " << endl;
				emit fetchedGameList(e);
			}
			else if (e.tagName() == "playerlist")
			{
				cout << "found playerlist " << endl;
				emit fetchedPlayerList(e);
			}
			if (e.tagName() == "game")
			{
				cout << "found game     ! " << endl;
			}
			a = e.attributeNode(QString("id"));
			if(!a.isNull())
				cout << "node has attr  : (id) : " << a.value() << endl;
		}
		QDomNode node = n.firstChild();
		processNode(node);
		n = n.nextSibling();
	}
}
