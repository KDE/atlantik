#include <qcombobox.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <kdualcolorbtn.h>
#include <qlineedit.h>
#include <klocale.h>
#include <qlayout.h>

#include "editor.h"

EstateEdit::EstateEdit(QWidget *parent, const char *name) : QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(this, 6);
	nameEdit = new QLineEdit(this, "Name Edit");
	layout->addWidget(nameEdit);
	
	fgbgButton = new KDualColorButton(this, "Foreground/Background Button");
	layout->addWidget(fgbgButton);
	layout->addStretch();

	QHBoxLayout *typeLayout = new QHBoxLayout(layout, 6);
	QLabel *typeLabel = new QLabel(i18n("Type"), this);
	typeLayout->addWidget(typeLabel);
	typeCombo = new QComboBox(false, this, "Type Combo");
	typeLayout->addWidget(typeCombo);

	QStringList types(i18n("Street"));
	types.append(i18n("Airport"));
	types.append(i18n("Chance"));
	types.append(i18n("Street"));
	types.append(i18n("Go"));
	types.append(i18n("Jail"));
	types.append(i18n("Todo"));
	typeCombo->insertStringList(types);
}

#include "editor.moc"
