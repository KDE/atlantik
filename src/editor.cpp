#include <qcombobox.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <kcolorbutton.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <klocale.h>
#include <kdebug.h>
#include <qlayout.h>

#include "estate.h"

#include "board.h"
#include "editor.h"

ConfigEstate::ConfigEstate(int estateId) : Estate(estateId)
{
}

void ConfigEstate::setType(const int type)
{
	if (m_type != type)
	{
		m_type = type;
		m_changed = true;
	}
}

///////////////////////////

EstateEdit::EstateEdit(QWidget *parent, const char *name) : AtlantikBoard(parent, name)
{
	connect(this, SIGNAL(somethingChanged()), this, SLOT(saveEstate()));

	QVBoxLayout *layout = new QVBoxLayout(m_center, 6);
	nameEdit = new QLineEdit(m_center, "Name Edit");
	layout->addWidget(nameEdit);
	connect(nameEdit, SIGNAL(returnPressed()), this, SIGNAL(somethingChanged()));

	layout->addStretch();

	QGroupBox *colorGroupBox = new QGroupBox(i18n("Colors"), m_center);
	layout->addWidget(colorGroupBox);
	
	QGridLayout *colorLayout = new QGridLayout(colorGroupBox, 2, 2, 6);
	colorLayout->addWidget(new QLabel(i18n("Foreground"), colorGroupBox), 0, 0);
	fgButton = new KColorButton(colorGroupBox, "Foreground Button");
	colorLayout->addWidget(fgButton, 1, 0);
	connect(fgButton, SIGNAL(changed(const QColor &)), this, SIGNAL(somethingChanged()));
	colorLayout->addWidget(new QLabel(i18n("Background"), colorGroupBox), 0, 1);
	bgButton = new KColorButton(colorGroupBox, "Background Button");
	colorLayout->addWidget(bgButton, 1, 1);
	connect(bgButton, SIGNAL(changed(const QColor &)), this, SIGNAL(somethingChanged()));

	QHBoxLayout *typeLayout = new QHBoxLayout(layout, 6);
	QLabel *typeLabel = new QLabel(i18n("Type"), m_center);
	typeLayout->addWidget(typeLabel);
	typeCombo = new QComboBox(false, m_center, "Type Combo");
	typeLayout->addWidget(typeCombo);
	connect(typeCombo, SIGNAL(activated(int)), this, SIGNAL(somethingChanged()));

	QStringList types(i18n("Street"));
	types.append(i18n("Airport"));
	types.append(i18n("Chance"));
	types.append(i18n("Street"));
	types.append(i18n("Go"));
	types.append(i18n("Jail"));
	types.append(i18n("Go to jail"));
	types.append("TODO");
	typeCombo->insertStringList(types);
}

void EstateEdit::setEstate(ConfigEstate *estate)
{
	typeCombo->setCurrentItem(estate->type());
	nameEdit->setText(estate->name());
	fgButton->setColor(estate->color());
	bgButton->setColor(estate->bgColor());
	this->estate = estate;
}

ConfigEstate *EstateEdit::saveEstate()
{
	if (!estate)
		return 0;
	estate->setType(typeCombo->currentItem());
	kdDebug() << "estate->type = " << estate->type() << endl;
	estate->setName(nameEdit->text());
	estate->setColor(fgButton->color());
	estate->setBgColor(bgButton->color());

	estate->update();
	return estate;
}

#include "editor.moc"
