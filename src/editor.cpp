#include <qcombobox.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <kdialogbase.h>
#include <kcolorbutton.h>
#include <qlineedit.h>
#include <qvgroupbox.h>
#include <klocale.h>
#include <kdebug.h>
#include <qlayout.h>

#include "estate.h"

#include "board.h"
#include "editor.h"

ConfigEstate::ConfigEstate(int estateId) : Estate(estateId)
{
	m_rent[0] = m_rent[1] = m_rent[2] = m_rent[3] = m_rent[4] = m_rent[5] = 0;
	m_type = 0;
	m_group = -1;
}

void ConfigEstate::setChanged(bool b)
{
	m_changed = b;
}

void ConfigEstate::setType(const int type)
{
	if (m_type != type)
	{
		m_type = type;
		m_changed = true;
	}
}

void ConfigEstate::setGroup(const int group)
{
	if (m_group != group)
	{
		m_group = group;
		m_changed = true;
	}
}

void ConfigEstate::setPrice(const int price)
{
	if (m_price != price)
	{
		m_price = price;
		m_changed = true;
	}
}

void ConfigEstate::setHousePrice(const int housePrice)
{
	if (m_housePrice != housePrice)
	{
		m_housePrice = housePrice;
		m_changed = true;
	}
}

void ConfigEstate::setRent(const int houses, const int rent)
{
	if (m_rent[houses] != rent)
	{
		m_rent[houses] = rent;
		m_changed = true;
	}
}

void ConfigEstate::setTax(const int tax)
{
	if (m_tax != tax)
	{
		m_tax = tax;
		m_changed = true;
	}
}

void ConfigEstate::setTaxPercentage(const int taxPercentage)
{
	if (m_taxPercentage != taxPercentage)
	{
		m_taxPercentage = taxPercentage;
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

	layout->addStretch(2);

	QVGroupBox *colorGroupBox = new QVGroupBox(i18n("Colors"), m_center);
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

	layout->addStretch(3);

	QHBoxLayout *typeLayout = new QHBoxLayout(layout, 6);
	QLabel *typeLabel = new QLabel(i18n("Type"), m_center);
	typeLayout->addWidget(typeLabel);
	typeCombo = new QComboBox(false, m_center, "Type Combo");
	typeLayout->addWidget(typeCombo);
	connect(typeCombo, SIGNAL(activated(int)), this, SIGNAL(somethingChanged()));

	configureButton = new QPushButton(m_center);
	layout->addWidget(configureButton);
	connect(configureButton, SIGNAL(clicked()), this, SLOT(configure()));

	QStringList types(i18n("Street"));
	types.append(i18n("Railroad"));
	types.append(i18n("Utility"));
	types.append(i18n("Community Chest"));
	types.append(i18n("Chance"));
	types.append(i18n("Free Parking"));
	types.append(i18n("Go to jail"));
	types.append(i18n("Tax"));
	types.append(i18n("Airport"));
	types.append(i18n("Jail"));
	types.append(i18n("Go"));
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

	saveEstate();
}

ConfigEstate *EstateEdit::saveEstate()
{
	if (!estate)
		return 0;
	int curType = typeCombo->currentItem();
	estate->setType(curType);
	estate->setName(nameEdit->text());
	estate->setColor(fgButton->color());
	estate->setBgColor(bgButton->color());

	if (curType != Street)
	{
		fgButton->setEnabled(false);
		fgButton->setColor(QColor("zzzzzz"));
		estate->setColor(QColor("zzzzzz"));
	}
	else
		fgButton->setEnabled(true);

	configureButton->setText(i18n("Configure %1 properties...").arg(typeCombo->currentText()));
	configureButton->setEnabled(curType == Street || curType == Tax);

	estate->update();
	return estate;
}

void EstateEdit::configure()
{
	KDialogBase *dialog;
	switch (typeCombo->currentItem())
	{
		case Street:
		dialog = new StreetDlg(estate, this);
		break;
		case Tax:
		dialog = new TaxDlg(estate, this);
		break;
		default:
		return;
		break;
	}
	dialog->show();
}

bool EstateEdit::upArrow()
{
	return (nameEdit->hasFocus() || typeCombo->hasFocus());
}

bool EstateEdit::downArrow()
{
	return upArrow();
}

bool EstateEdit::leftArrow()
{
	if (nameEdit->hasFocus())
	{
		nameEdit->setCursorPosition(nameEdit->cursorPosition() - 1);
		return true;
	}
	return false;
}

bool EstateEdit::rightArrow()
{
	if (nameEdit->hasFocus())
	{
		nameEdit->setCursorPosition(nameEdit->cursorPosition() + 1);
		return true;
	}
	return false;
}

/////////////////////////////////

TaxDlg::TaxDlg(ConfigEstate *estate, QWidget *parent, char *name, bool modal)
	:KDialogBase(KDialogBase::TreeList, i18n("Configure %1").arg(estate->name()), Ok|Cancel, Ok, parent, name, modal)
{
	this->estate = estate;

	setHelp("atlantik/index.html", QString::null);

	QFrame *TaxPage = addPage(i18n("Tax"));
	QGridLayout *taxBox = new QGridLayout(TaxPage, 2, 2);
	taxBox->addWidget(new QLabel(i18n("Fixed tax"), TaxPage), 0, 0);
	taxBox->addWidget(tax = new QSpinBox(0, 3000, 1, TaxPage), 0, 1);
	tax->setSuffix("$");
	tax->setValue(estate->tax());
	taxBox->addWidget(new QLabel(i18n("Percentage tax"), TaxPage), 1, 0);
	taxBox->addWidget(taxPercentage = new QSpinBox(0, 100, 1, TaxPage), 1, 1);
	taxPercentage->setSuffix("%");
	taxPercentage->setValue(estate->taxPercentage());
}

void TaxDlg::slotOk()
{
	estate->setTax(tax->value());
	estate->setTaxPercentage(taxPercentage->value());
	KDialogBase::slotOk();
}

/////////////////////////////////

StreetDlg::StreetDlg(ConfigEstate *estate, QWidget *parent, char *name, bool modal)
	:KDialogBase(KDialogBase::TreeList, i18n("Configure %1").arg(estate->name()), Ok|Cancel, Ok, parent, name, modal)
{
	this->estate = estate;

	setHelp("atlantik/index.html", QString::null);

	QFrame *RentPage = addPage(i18n("Rent"));
	QGridLayout *rentBox = new QGridLayout(RentPage, 6, 2);
	rentBox->addWidget(new QLabel(i18n("No houses"), RentPage), 0, 0);
	rentBox->addWidget(new QLabel(i18n("One house"), RentPage), 1, 0);
	rentBox->addWidget(new QLabel(i18n("Two houses"), RentPage), 2, 0);
	rentBox->addWidget(new QLabel(i18n("Three houses"), RentPage), 3, 0);
	rentBox->addWidget(new QLabel(i18n("Four houses"), RentPage), 4, 0);
	rentBox->addWidget(new QLabel(i18n("One hotel"), RentPage), 5, 0);
	rentBox->addWidget(houses0 = new QSpinBox(0, 3000, 1, RentPage), 0, 1);
	rentBox->addWidget(houses1 = new QSpinBox(0, 3000, 1, RentPage), 1, 1);
	rentBox->addWidget(houses2 = new QSpinBox(0, 3000, 1, RentPage), 2, 1);
	rentBox->addWidget(houses3 = new QSpinBox(0, 3000, 1, RentPage), 3, 1);
	rentBox->addWidget(houses4 = new QSpinBox(0, 3000, 1, RentPage), 4, 1);
	rentBox->addWidget(houses5 = new QSpinBox(0, 3000, 1, RentPage), 5, 1);
	houses0->setSuffix(i18n("$"));
	houses1->setSuffix(i18n("$"));
	houses2->setSuffix(i18n("$"));
	houses3->setSuffix(i18n("$"));
	houses4->setSuffix(i18n("$"));
	houses5->setSuffix(i18n("$"));
	houses0->setValue(estate->rent(0));
	houses1->setValue(estate->rent(1));
	houses2->setValue(estate->rent(2));
	houses3->setValue(estate->rent(3));
	houses4->setValue(estate->rent(4));
	houses5->setValue(estate->rent(5));

	QFrame *PricesPage = addPage(i18n("Prices"));
	QGridLayout *pricesBox = new QGridLayout(PricesPage, 2, 2);
	pricesBox->addWidget(new QLabel(i18n("Price"), PricesPage), 0, 0);
	pricesBox->addWidget(price = new QSpinBox(0, 3000, 25, PricesPage), 0, 1);
	price->setSuffix(i18n("$"));
	price->setValue(estate->price());
	pricesBox->addWidget(new QLabel(i18n("House price"), PricesPage), 1, 0);
	pricesBox->addWidget(housePrice = new QSpinBox(0, 3000, 25, PricesPage), 1, 1);
	housePrice->setSuffix(i18n("$"));
	housePrice->setValue(estate->housePrice());

	QFrame *GroupPage = addPage(i18n("Group"));
	QHBoxLayout *groupLayout = new QHBoxLayout(GroupPage, 6);
	QLabel *groupLabel = new QLabel(i18n("Group"), GroupPage);
	groupLayout->addWidget(groupLabel);
	groupCombo = new QComboBox(false, GroupPage, "Group Combo");
	QStringList groups(i18n("None"));
	for (int i = 0; i <= 20; i++)
	{
		groups.append(QString::number(i));
	}
	groupCombo->insertStringList(groups);
	groupLayout->addWidget(groupCombo);
	groupCombo->setCurrentItem(estate->group() + 1);
}

void StreetDlg::slotOk()
{
	estate->setRent(0, houses0->value());
	estate->setRent(1, houses1->value());
	estate->setRent(2, houses2->value());
	estate->setRent(3, houses3->value());
	estate->setRent(4, houses4->value());
	estate->setRent(5, houses5->value());
	estate->setPrice(price->value());
	estate->setHousePrice(housePrice->value());
	estate->setGroup(groupCombo->currentItem() - 1);
	KDialogBase::slotOk();
}

#include "editor.moc"
