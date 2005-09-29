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

#include <qlayout.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qlabel.h>

#include <kdeversion.h>
#undef KDE_3_1_FEATURES
#undef KDE_3_3_FEATURES
#if defined(KDE_MAKE_VERSION)
#if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
#define KDE_3_1_FEATURES
#endif
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,90)
#define KDE_3_3_FEATURES
#endif
#endif

#include <kicondialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

#include "atlantik.h"
#include "configdlg.moc"

ConfigDialog::ConfigDialog(Atlantik* parent, const char *name) : KDialogBase(IconList, i18n("Configure Atlantik"), Ok|Cancel, Ok, parent, "config_atlantik", false, name)
{
	m_parent = parent;
	p_general = addPage(i18n("General"), i18n("General"), BarIcon("configure", KIcon::SizeMedium));
	p_p13n = addPage(i18n("Personalization"), i18n("Personalization"), BarIcon("personal", KIcon::SizeMedium));
	p_board = addPage(i18n("Board"), i18n("Board"), BarIcon("monop_board", KIcon::SizeMedium));
	p_monopigator = addPage(i18n("Meta Server"), i18n("Meta Server"), BarIcon("network", KIcon::SizeMedium));

	configGeneral = new ConfigGeneral(this, p_general, "configGeneral");
	configPlayer = new ConfigPlayer(this, p_p13n, "configPlayer");
	configBoard = new ConfigBoard(this, p_board, "configBoard");
	configMonopigator = new ConfigMonopigator(this, p_monopigator, "configMonopigator");

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

ConfigPlayer::ConfigPlayer(ConfigDialog* configDialog, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_configDialog = configDialog;
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	QLabel *label = new QLabel(i18n("Player name:"), parent);
	layout->addWidget(label);

	m_playerName = new QLineEdit(parent);
	layout->addWidget(m_playerName);

	QLabel *label2 = new QLabel(i18n("Player image:"), parent);
	layout->addWidget(label2);
                
	m_playerIcon = new KPushButton(parent, "playerIcon");
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
	KIconDialog iconDialog( this, "iconDialog" );
#ifdef KDE_3_1_FEATURES
	iconDialog.setCustomLocation( locate("appdata", "themes/default/tokens/") );
#endif
		
#ifdef KDE_3_3_FEATURES
	iconDialog.setup( KIcon::Desktop, KIcon::Application, false, 0, true, true, true ); // begin with user icons, lock editing
#else
	iconDialog.setup( KIcon::Desktop, KIcon::Application, false, 0, true ); // begin with user icons
#endif

	QString image = iconDialog.openDialog();

	if ( image.isEmpty() )
		return;

	QStringList splitPath = QStringList::split( '/', image );
	m_playerImage = splitPath[ splitPath.count()-1 ];

	setImage();
}

void ConfigPlayer::setImage()
{
	QString filename = locate("data", "atlantik/themes/default/tokens/" + m_playerImage);
	if (KStandardDirs::exists(filename))
		m_playerIcon->setPixmap( QPixmap(filename) );
}
															
void ConfigPlayer::reset()
{
	m_playerName->setText(m_configDialog->config().playerName);
	m_playerImage = m_configDialog->config().playerImage;
	setImage();
}

ConfigMonopigator::ConfigMonopigator(ConfigDialog *configDialog, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_configDialog = configDialog;
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	m_connectOnStart = new QCheckBox(i18n("Request list of Internet servers on start-up"), parent);
	layout->addWidget(m_connectOnStart);

	QString message=i18n(
		"If checked, Atlantik connects to a meta server on start-up to\n"
		"request a list of Internet servers.\n");
	QWhatsThis::add(m_connectOnStart, message);

	m_hideDevelopmentServers = new QCheckBox(i18n("Hide development servers"), parent);
	layout->addWidget(m_hideDevelopmentServers);

	message=i18n(
		"Some of the Internet servers might be running development\n"
		"versions of the server software. If checked, Atlantik will not\n"
		"display these servers.\n");
	QWhatsThis::add(m_hideDevelopmentServers, message);

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

ConfigGeneral::ConfigGeneral(ConfigDialog *configDialog, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_configDialog = configDialog;
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	m_chatTimestamps = new QCheckBox(i18n("Show timestamps in chat messages"), parent);
	layout->addWidget(m_chatTimestamps);

	QString message=i18n(
		"If checked, Atlantik will add timestamps in front of chat\n"
		"messages.\n");
	QWhatsThis::add(m_chatTimestamps, message);

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

ConfigBoard::ConfigBoard(ConfigDialog *configDialog, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_configDialog = configDialog;
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	QGroupBox *box = new QGroupBox(1, Qt::Horizontal, i18n("Game Status Feedback"), parent);
	layout->addWidget(box);

	m_indicateUnowned = new QCheckBox(i18n("Display title deed card on unowned properties"), box);
	QString message=i18n(
		"If checked, unowned properties on the board display an estate\n"
		"card to indicate the property is for sale.\n");
	QWhatsThis::add(m_indicateUnowned, message);

	m_highliteUnowned = new QCheckBox(i18n("Highlight unowned properties"), box);
	message=i18n(
		"If checked, unowned properties on the board are highlighted to\n"
		"indicate the property is for sale.\n");
	QWhatsThis::add(m_highliteUnowned, message);

	m_darkenMortgaged = new QCheckBox(i18n("Darken mortgaged properties"), box);
	message=i18n(
		"If checked, mortgaged properties on the board will be colored\n"
		"darker than of the default color.\n");
	QWhatsThis::add(m_darkenMortgaged, message);

	m_animateToken = new QCheckBox(i18n("Animate token movement"), box);
	message=i18n(
		"If checked, tokens will move across the board\n"
		"instead of jumping directly to their new location.\n");
	QWhatsThis::add(m_animateToken, message);

	m_quartzEffects = new QCheckBox(i18n("Quartz effects"), box);
	message=i18n(
		"If checked, the colored headers of street estates on the board "
		"will have a Quartz effect similar to the Quartz KWin style.\n");
	QWhatsThis::add(m_quartzEffects, message);

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
