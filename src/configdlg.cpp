#include <qlayout.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>

#include <kiconloader.h>
#include <klocale.h>

#include "configdlg.moc"

ConfigDialog::ConfigDialog(QWidget* parent, const char *name) : KDialogBase(IconList, i18n("Configure KMonop"), Ok|Cancel, Ok, parent, "config_kmonop", false)
{
//	_options = opts;

	QFrame *page = addPage(QString(i18n("Board")), QString(i18n("Board")), BarIcon("monop_board", KIcon::SizeMedium));

	ConfigBoard *configBoard = new ConfigBoard(page, "configBoard");

	setMinimumSize(sizeHint());
}

ConfigBoard::ConfigBoard(QWidget* parent, const char *name) : QWidget(parent, name)
{
//	_options = opts;

	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	QGroupBox *box = new QGroupBox(1, Qt::Horizontal, i18n("Game status feedback"), parent);
	layout->addWidget(box);

	indicateUnowned = new QCheckBox(i18n("Indicate whether a property is unowned"), box);
	QString message=i18n(
		"Indicate whether a property is unowned\n\n"
		"If checked, unowned properties on the board\n"
		"display an estate card to indicate the\n"
		"property is for sale.\n");
	QWhatsThis::add(indicateUnowned, message);

	animateToken = new QCheckBox(i18n("Animate token movement"), box);
	message=i18n(
		"Animate token movement\n\n"
		"If checked, tokens will move across the board\n"
		"instead of jumping directly to their new location.\n");
	QWhatsThis::add(animateToken, message);

	box = new QGroupBox(1, Qt::Horizontal, i18n("Size"), parent);
	layout->addWidget(box);

	reset();
}

void ConfigBoard::reset()
{
	indicateUnowned->setChecked(_options.indicateUnowned);
	animateToken->setChecked(_options.animateToken);
}
