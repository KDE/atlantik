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
					if (a.value() == "startgame")
						emit msgStartGame(e.attributeNode(QString("value")).value());
				}
			}
			else if (e.tagName() == "gamelist")
				emit fetchedGameList(e);
			else if (e.tagName() == "playerlist")
				emit fetchedPlayerList(e);
		}
		QDomNode node = n.firstChild();
		processNode(node);
		n = n.nextSibling();
	}
}
