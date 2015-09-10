// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include <QLayout>
#include <qgroupbox.h>

#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

#include <kdeversion.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kicon.h>

#include "atlantik.h"
#include "configdlg.moc"

ConfigDialog::ConfigDialog(Atlantik* parent) : 
		KPageDialog(parent)
{
    setCaption(i18n("Configure Atlantik"));
    setButtons(Ok|Cancel);
    setDefaultButton(Ok);
    setModal(false);
    setFaceType(KPageDialog::List);
    m_parent = parent;
	configGeneral = new ConfigGeneral(this, 0);
	configGeneral->setObjectName("configGeneral");
	KPageWidgetItem *pageItem = new KPageWidgetItem(configGeneral, i18n("General"));
    pageItem->setHeader(i18n("General"));
	pageItem->setIcon(KIcon("configure"));
    addPage(pageItem);

	configPlayer = new ConfigPlayer(this, 0);
	configPlayer->setObjectName("configPlayer");
	pageItem = new KPageWidgetItem(configPlayer, i18n("Personalization"));
    pageItem->setHeader(i18n("Personalization"));
	pageItem->setIcon(KIcon("user-identity"));
    addPage(pageItem);	
	
	configBoard = new ConfigBoard(this, 0);
	configBoard->setObjectName("configBoard" );
    pageItem = new KPageWidgetItem(configBoard, i18n("Board"));
    pageItem->setHeader(i18n("Board"));
	pageItem->setIcon(KIcon("monop_board"));
    addPage(pageItem);
	
	configMonopigator = new ConfigMonopigator(this, 0);
	configMonopigator->setObjectName("configMonopigator");
	pageItem = new KPageWidgetItem(configMonopigator, i18n("Meta Server"));
    pageItem->setHeader(i18n("Meta Server"));
	pageItem->setIcon(KIcon("network-wired"));
    addPage(pageItem);

	setMinimumSize(sizeHint());
}

bool ConfigDialog::chatTimestamps()
{
	return configGeneral->chatTimestamps();
}

bool ConfigDialog::indicateUnowned()
{
	return configBoard->indicateUnowned();
}

bool ConfigDialog::highliteUnowned()
{
	return configBoard->highliteUnowned();
}

bool ConfigDialog::darkenMortgaged()
{
	return configBoard->darkenMortgaged();
}

bool ConfigDialog::animateToken()
{
	return configBoard->animateToken();
}

bool ConfigDialog::quartzEffects()
{
	return configBoard->quartzEffects();
}

QString ConfigDialog::playerName()
{
	return configPlayer->playerName();
}

QString ConfigDialog::playerImage()
{
	return configPlayer->playerImage();
}

bool ConfigDialog::connectOnStart()
{
	return configMonopigator->connectOnStart();
}

bool ConfigDialog::hideDevelopmentServers()
{
	return configMonopigator->hideDevelopmentServers();
}

AtlantikConfig ConfigDialog::config()
{
	return m_parent->config();
}

ConfigPlayer::ConfigPlayer(ConfigDialog* configDialog, QWidget *parent) : QWidget(parent)
{
	m_configDialog = configDialog;
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);

	QLabel *label = new QLabel(i18n("Player name:"), parent);
	layout->addWidget(label);

	m_playerName = new QLineEdit(parent);
	layout->addWidget(m_playerName);

	QLabel *label2 = new QLabel(i18n("Player image:"), parent);
	layout->addWidget(label2);

	m_playerIcon = new KPushButton(parent);
	layout->addWidget(m_playerIcon);

	connect( m_playerIcon, SIGNAL(clicked()), this, SLOT(chooseImage()) );

	layout->addStretch(1);

	reset();
}

QString ConfigPlayer::playerName()
{
	return m_playerName->text();
}

QString ConfigPlayer::playerImage()
{
	return m_playerImage;
}
void ConfigPlayer::chooseImage()
{
	KIconDialog iconDialog( this);
	iconDialog.setCustomLocation( KStandardDirs::locate("appdata", "themes/default/tokens/") );

	iconDialog.setup( KIconLoader::Desktop, KIconLoader::Application, false, 0, true, true, true ); // begin with user icons, lock editing

	QString image = iconDialog.openDialog();

	if ( image.isEmpty() )
		return;

	QStringList splitPath = image.split( '/', QString::SkipEmptyParts );
	m_playerImage = splitPath[ splitPath.count()-1 ];

	setImage();
}

void ConfigPlayer::setImage()
{
	QString filename = KStandardDirs::locate("data", "atlantik/themes/default/tokens/" + m_playerImage);
	if (KStandardDirs::exists(filename))
	{
		const QPixmap p(filename);
		m_playerIcon->setIcon(QIcon(p));
		m_playerIcon->setIconSize(p.size());
	}
}

void ConfigPlayer::reset()
{
	m_playerName->setText(m_configDialog->config().playerName);
	m_playerImage = m_configDialog->config().playerImage;
	setImage();
}

ConfigMonopigator::ConfigMonopigator(ConfigDialog *configDialog, QWidget *parent) : QWidget(parent)
{
	m_configDialog = configDialog;
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);

	m_connectOnStart = new QCheckBox(i18n("Request list of Internet servers on start-up"), parent);
	layout->addWidget(m_connectOnStart);

	QString message=i18n(
		"If checked, Atlantik connects to a meta server on start-up to\n"
		"request a list of Internet servers.\n");
	m_connectOnStart->setWhatsThis( message);

	m_hideDevelopmentServers = new QCheckBox(i18n("Hide development servers"), parent);
	layout->addWidget(m_hideDevelopmentServers);

	message=i18n(
		"Some of the Internet servers might be running development\n"
		"versions of the server software. If checked, Atlantik will not\n"
		"display these servers.\n");
	m_hideDevelopmentServers->setWhatsThis( message);

	layout->addStretch(1);

	reset();
}

bool ConfigMonopigator::connectOnStart()
{
	return m_connectOnStart->isChecked();
}

bool ConfigMonopigator::hideDevelopmentServers()
{
	return m_hideDevelopmentServers->isChecked();
}

void ConfigMonopigator::reset()
{
	m_connectOnStart->setChecked(m_configDialog->config().connectOnStart);
	m_hideDevelopmentServers->setChecked(m_configDialog->config().hideDevelopmentServers);
}

ConfigGeneral::ConfigGeneral(ConfigDialog *configDialog, QWidget *parent) : QWidget(parent)
{
	m_configDialog = configDialog;
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);

	m_chatTimestamps = new QCheckBox(i18n("Show timestamps in chat messages"), parent);
	layout->addWidget(m_chatTimestamps);

	QString message=i18n(
		"If checked, Atlantik will add timestamps in front of chat\n"
		"messages.\n");
	m_chatTimestamps->setWhatsThis( message);

	layout->addStretch(1);

	reset();
}

bool ConfigGeneral::chatTimestamps()
{
	return m_chatTimestamps->isChecked();
}

void ConfigGeneral::reset()
{
	m_chatTimestamps->setChecked(m_configDialog->config().chatTimestamps);
}

ConfigBoard::ConfigBoard(ConfigDialog *configDialog, QWidget *parent) : QWidget(parent)
{
	m_configDialog = configDialog;
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);

	QGroupBox *box = new QGroupBox(i18n("Game Status Feedback"), parent);
	layout->addWidget(box);

	QVBoxLayout *boxLayout = new QVBoxLayout(box);

	m_indicateUnowned = new QCheckBox(i18n("Display title deed card on unowned properties"), box);
	boxLayout->addWidget(m_indicateUnowned);
	QString message=i18n(
		"If checked, unowned properties on the board display an estate\n"
		"card to indicate the property is for sale.\n");
	m_indicateUnowned->setWhatsThis( message);

	m_highliteUnowned = new QCheckBox(i18n("Highlight unowned properties"), box);
	boxLayout->addWidget(m_highliteUnowned);
	message=i18n(
		"If checked, unowned properties on the board are highlighted to\n"
		"indicate the property is for sale.\n");
	m_highliteUnowned->setWhatsThis( message);

	m_darkenMortgaged = new QCheckBox(i18n("Darken mortgaged properties"), box);
	boxLayout->addWidget(m_darkenMortgaged);
	message=i18n(
		"If checked, mortgaged properties on the board will be colored\n"
		"darker than of the default color.\n");
	m_darkenMortgaged->setWhatsThis( message);

	m_animateToken = new QCheckBox(i18n("Animate token movement"), box);
	boxLayout->addWidget(m_animateToken);
	message=i18n(
		"If checked, tokens will move across the board\n"
		"instead of jumping directly to their new location.\n");
	m_animateToken->setWhatsThis( message);

	m_quartzEffects = new QCheckBox(i18n("Quartz effects"), box);
	boxLayout->addWidget(m_quartzEffects);
	message=i18n(
		"If checked, the colored headers of street estates on the board "
		"will have a Quartz effect similar to the Quartz KWin style.\n");
	m_quartzEffects->setWhatsThis( message);

//	box = new QGroupBox(1, Qt::Horizontal, i18n("Size"), parent);
//	layout->addWidget(box);

	layout->addStretch(1);

	reset();
}

bool ConfigBoard::indicateUnowned()
{
	return m_indicateUnowned->isChecked();
}

bool ConfigBoard::highliteUnowned()
{
	return m_highliteUnowned->isChecked();
}

bool ConfigBoard::darkenMortgaged()
{
	return m_darkenMortgaged->isChecked();
}

bool ConfigBoard::animateToken()
{
	return m_animateToken->isChecked();
}

bool ConfigBoard::quartzEffects()
{
	return m_quartzEffects->isChecked();
}

void ConfigBoard::reset()
{
	m_indicateUnowned->setChecked(m_configDialog->config().indicateUnowned);
	m_highliteUnowned->setChecked(m_configDialog->config().highliteUnowned);
	m_darkenMortgaged->setChecked(m_configDialog->config().darkenMortgaged);
	m_animateToken->setChecked(m_configDialog->config().animateTokens);
	m_quartzEffects->setChecked(m_configDialog->config().quartzEffects);
}
