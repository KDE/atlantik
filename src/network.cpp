#include <iostream.h>

#include "network.moc"

GameNetwork::GameNetwork(QObject *parent, const char *name) : QSocket(parent, name)
{
	//
}

void GameNetwork::slotWrite(const char *input)
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
		cout << "[[[ " + str + " ]]] " << endl;
		processCmd(str);
	}
}

void GameNetwork::processCmd(QString str)
{
	cout << "processing: " + str << endl;
	if (str.startsWith(QString("<gamelist>")))
		cout << "we have a gamelist!" << endl;
}
