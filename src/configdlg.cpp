#include <qlayout.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>

#include <kiconloader.h>
#include <klocale.h>

#include "configdlg.moc"

ConfigDialog::ConfigDialog(QWidget* parent, const char *name) : KDialogBase(IconList, i18n("Configure KMonop"), Ok|Cancel, Ok, parent, "config_kmonop", false)
{
	QFrame *page = addPage(QString(i18n("Board")), QString(i18n("Board")), BarIcon("monop_board", KIcon::SizeMedium));
	ConfigBoard *configBoard = new ConfigBoard(page, "configBoard");

	setMinimumSize(sizeHint());
}

ConfigBoard::ConfigBoard(QWidget* parent, const char *name) : QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	QGroupBox *box = new QGroupBox(1, Qt::Horizontal, i18n("Game status feedback"), parent);
	layout->addWidget(box);

	QCheckBox *indicateUnowned = new QCheckBox(i18n("Indicate whether a property is unowned"), box);
	QString message=i18n(
		"Indicate whether a property is unowned\n\n"
		"If checked, unowned properties on the board\n"
		"display an estate card to indicate the\n"
		"property is for sale.\n");
	QWhatsThis::add(indicateUnowned, message);

	box = new QGroupBox(1, Qt::Horizontal, i18n("Size"), parent);
	layout->addWidget(box);
}
