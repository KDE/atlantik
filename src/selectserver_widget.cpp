#include <qvbuttongroup.h>
#include <qradiobutton.h>

#include <klocale.h>
#include <kiconloader.h>

#include "selectserver_widget.moc"

SelectServer::SelectServer(QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, 10);
	CHECK_PTR(m_mainLayout);

	QVButtonGroup *bgroup;
//	bgroup = new QVButtonGroup(i18n("Start or select a monopd server"), this, "bgroup");
	bgroup = new QVButtonGroup(i18n("Select a monopd server"), this, "bgroup");
	bgroup->setExclusive(true);
	m_mainLayout->addWidget(bgroup); 

	// Button for local games
//	m_localGameButton = new QRadioButton(QString(i18n("Start a local server")), bgroup, "m_localGameButton");
//	connect(m_localGameButton, SIGNAL(stateChanged(int)), this, SLOT(validateConnectButton())); 

	// Button for on-line games
//	m_onlineGameButton = new QRadioButton(QString(i18n("Select a server to play a game on-line")), bgroup, "m_onlineGameButton");
//	m_localGameButton->setEnabled(true);
//	connect(m_onlineGameButton, SIGNAL(stateChanged(int)), this, SLOT(validateConnectButton()));

	// List of servers
	m_serverList = new QListView(bgroup, "m_serverList");
	m_serverList->addColumn(QString(i18n("Host")));
	m_serverList->addColumn(QString(i18n("Port")));
	m_serverList->addColumn(QString(i18n("Version")));
//	m_mainLayout->addWidget(m_serverList);

	connect(m_serverList, SIGNAL(clicked(QListViewItem *)), this, SLOT(validateConnectButton()));
//	connect(m_serverList, SIGNAL(clicked(QListViewItem *)), this, SLOT(slotListClicked(QListViewItem *)));
	connect(m_serverList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(connectPressed()));
	connect(m_serverList, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)), this, SLOT(validateConnectButton()));
	connect(m_serverList, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(validateConnectButton()));

	m_refreshButton = new QPushButton(BarIcon("reload", 16), i18n("Refresh"), bgroup);

	connect(m_refreshButton, SIGNAL(pressed()), this, SLOT(initMonopigator()));

	m_connectButton = new QPushButton(BarIcon("forward", 16), i18n("Connect"), this);
	m_connectButton->setEnabled(false);
	m_mainLayout->addWidget(m_connectButton);

	connect(m_connectButton, SIGNAL(pressed()), this, SLOT(connectPressed()));
	
    // Status indicator
	status_label = new QLabel(this);
	m_mainLayout->addWidget(status_label);

	// Monopigator
	monopigator = new Monopigator();

	connect(monopigator, SIGNAL(monopigatorClear()), this, SLOT(slotMonopigatorClear()));
	connect(monopigator, SIGNAL(monopigatorAdd(QString, QString, QString)), this, SLOT(slotMonopigatorAdd(QString, QString, QString)));

	// Until we have a good way to use start a local monopd server, disable this button
//	m_localGameButton->setEnabled(false);

	initMonopigator();
}

void SelectServer::initMonopigator()
{
	// Hardcoded, but there aren't any other Monopigator root servers at the moment
	status_label->setText(i18n("Retrieving server list..."));
	monopigator->loadData("http://gator.monopd.net/");
}

void SelectServer::slotMonopigatorClear()
{
	m_serverList->clear();
	validateConnectButton();
//	emit statusChanged();
}

void SelectServer::slotMonopigatorAdd(QString host, QString port, QString version)
{
	QListViewItem *item = new QListViewItem(m_serverList, host, port, version);
	item->setPixmap(0, BarIcon("atlantik", 16));
	validateConnectButton();
}

/*
void SelectServer::monopigatorEnd()
{
	status_label->setText(i18n("Retrieved server list."));
}
*/

void SelectServer::validateRadioButtons()
{
	return;
	if (m_serverList->childCount() > 0)
	{
		if (!m_onlineGameButton->isEnabled())
			m_onlineGameButton->setEnabled(true);
	}
	else
	{
		if (m_onlineGameButton->isEnabled())
			m_onlineGameButton->setEnabled(false);
		m_serverList->clearSelection();
	}
}

void SelectServer::validateConnectButton()
{
	validateRadioButtons();

//	if ( (m_localGameButton->isEnabled() && m_localGameButton->isChecked()) || (m_onlineGameButton->isEnabled() && m_onlineGameButton->isChecked() && m_serverList->selectedItem()) )
	if (m_serverList->selectedItem())
		m_connectButton->setEnabled(true);
	else
		m_connectButton->setEnabled(false);
}

void SelectServer::slotListClicked(QListViewItem *item)
{
	// Simulate a user press
	if(item && !m_onlineGameButton->isOn())
		m_onlineGameButton->toggle();
}

void SelectServer::connectPressed()
{
	if (QListViewItem *item = m_serverList->selectedItem())
		emit serverConnect(item->text(0), item->text(1).toInt());
}
