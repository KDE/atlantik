#include <qlayout.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qlabel.h>

#include <kiconloader.h>
#include <klocale.h>

#include "config.h"
#include "configdlg.moc"

extern KMonopConfig kmonopConfig;

ConfigDialog::ConfigDialog(QWidget* parent, const char *name) : KDialogBase(IconList, i18n("Configure KMonop"), Ok|Cancel, Ok, parent, "config_kmonop", false)
{
//	p_p13n = addPage(QString(i18n("Personalization")), QString(i18n("Personalization")), BarIcon("personal", KIcon::SizeMedium));
	p_board = addPage(QString(i18n("Board")), QString(i18n("Board")), BarIcon("monop_board", KIcon::SizeMedium));

//	configPlayer = new ConfigPlayer(p_p13n, "configPlayer");
	configBoard = new ConfigBoard(p_board, "configBoard");

	setMinimumSize(sizeHint());
}

bool ConfigDialog::indicateUnowned()
{
	return configBoard->_indicateUnowned->isChecked();
}

bool ConfigDialog::animateToken()
{
	return configBoard->_animateToken->isChecked();
}

QString ConfigDialog::playerName()
{
	return configPlayer->_playerName->text();
}

ConfigPlayer::ConfigPlayer(QWidget* parent, const char *name) : QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	QLabel *label = new QLabel("Player name:", parent);	
	layout->addWidget(label);

	_playerName = new QLineEdit(parent);
	layout->addWidget(_playerName);
	
	reset();
}

void ConfigPlayer::reset()
{
	_playerName->setText(kmonopConfig.playerName);
}

ConfigBoard::ConfigBoard(QWidget* parent, const char *name) : QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	QGroupBox *box = new QGroupBox(1, Qt::Horizontal, i18n("Game status feedback"), parent);
	layout->addWidget(box);

	_indicateUnowned = new QCheckBox(i18n("Indicate whether a property is unowned"), box);

	QString message=i18n(
		"Indicate whether a property is unowned\n\n"
		"If checked, unowned properties on the board\n"
		"display an estate card to indicate the\n"
		"property is for sale.\n");
	QWhatsThis::add(_indicateUnowned, message);

	_animateToken = new QCheckBox(i18n("Animate token movement"), box);

	message=i18n(
		"Animate token movement\n\n"
		"If checked, tokens will move across the board\n"
		"instead of jumping directly to their new location.\n");
	QWhatsThis::add(_animateToken, message);

	box = new QGroupBox(1, Qt::Horizontal, i18n("Size"), parent);
	layout->addWidget(box);

	reset();
}

void ConfigBoard::reset()
{
	_indicateUnowned->setChecked(kmonopConfig.indicateUnowned);
	_animateToken->setChecked(kmonopConfig.animateToken);
}
