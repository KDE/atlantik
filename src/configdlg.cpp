#include <qlayout.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qlabel.h>

#include <kiconloader.h>
#include <klocale.h>

#include "config.h"
#include "configdlg.moc"

extern KMonopConfig atlantikConfig;

ConfigDialog::ConfigDialog(QWidget* parent, const char *name) : KDialogBase(IconList, i18n("Configure KMonop"), Ok|Cancel, Ok, parent, "config_atlantik", false)
{
	p_p13n = addPage(QString(i18n("Personalization")), QString(i18n("Personalization")), BarIcon("personal", KIcon::SizeMedium));
	p_board = addPage(QString(i18n("Board")), QString(i18n("Board")), BarIcon("monop_board", KIcon::SizeMedium));

	configPlayer = new ConfigPlayer(p_p13n, "configPlayer");
	configBoard = new ConfigBoard(p_board, "configBoard");

	setMinimumSize(sizeHint());
}

bool ConfigDialog::indicateUnowned()
{
	return configBoard->m_indicateUnowned->isChecked();
}

bool ConfigDialog::highliteUnowned()
{
	return configBoard->m_highliteUnowned->isChecked();
}

bool ConfigDialog::grayOutMortgaged()
{
	return configBoard->m_grayOutMortgaged->isChecked();
}

bool ConfigDialog::animateToken()
{
	return configBoard->m_animateToken->isChecked();
}

bool ConfigDialog::quartzEffects()
{
	return configBoard->m_quartzEffects->isChecked();
}

QString ConfigDialog::playerName()
{
	return configPlayer->m_playerName->text();
}

ConfigPlayer::ConfigPlayer(QWidget* parent, const char *name) : QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	QLabel *label = new QLabel("Player name:", parent);	
	layout->addWidget(label);

	m_playerName = new QLineEdit(parent);
	layout->addWidget(m_playerName);

	layout->addStretch(1);

	reset();
}

void ConfigPlayer::reset()
{
	m_playerName->setText(atlantikConfig.playerName);
}

ConfigBoard::ConfigBoard(QWidget* parent, const char *name) : QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	QGroupBox *box = new QGroupBox(1, Qt::Horizontal, i18n("Game status feedback"), parent);
	layout->addWidget(box);

	m_indicateUnowned = new QCheckBox(i18n("Display Title Deed card on unowned properties"), box);
	QString message=i18n(
		"If checked, unowned properties on the board display an estate\n"
		"card to indicate the property is for sale.\n");
	QWhatsThis::add(m_indicateUnowned, message);

	m_highliteUnowned = new QCheckBox(i18n("Highlight unowned properties"), box);
	message=i18n(
		"If checked, unowned properties on the board are highlighted to\n"
		"indicate the property is for sale.\n");
	QWhatsThis::add(m_highliteUnowned, message);

	m_grayOutMortgaged = new QCheckBox(i18n("Gray out mortgaged properties"), box);
	message=i18n(
		"If checked, mortgaged properties on the board will be colored\n"
		"gray instead of the default color.\n");
	QWhatsThis::add(m_grayOutMortgaged, message);

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

void ConfigBoard::reset()
{
	m_indicateUnowned->setChecked(atlantikConfig.indicateUnowned);
	m_highliteUnowned->setChecked(atlantikConfig.highliteUnowned);
	m_grayOutMortgaged->setChecked(atlantikConfig.grayOutMortgaged);
	m_animateToken->setChecked(atlantikConfig.animateToken);
	m_quartzEffects->setChecked(atlantikConfig.quartzEffects);
}
