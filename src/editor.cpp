#include <qcombobox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qframe.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qspinbox.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qvgroupbox.h>
#include <qlayout.h>

#include <kdebug.h>
#include <klineeditdlg.h>
#include <kdialogbase.h>
#include <kcolorbutton.h>
#include <klocale.h>
#include <kpushbutton.h>

#include "editor.h"
#include "estate.h"

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

QStringList types;

EstateEdit::EstateEdit(CardStack *chanceStack, CardStack *ccStack, QWidget *parent, const char *name) : QWidget(parent, name)
{
	types.append("pay");
	types.append("payeach");
	types.append("collect");
	types.append("collecteach");
	types.append("advanceto");
	types.append("advance");
	types.append("goback");
	types.append("tojail");
	types.append("jailcard");
	types.append("nextutil");
	types.append("nextrr");
	types.append("payhouse");
	types.append("payhotel");

	this->chanceStack = chanceStack;
	this->ccStack = ccStack;

	connect(this, SIGNAL(somethingChanged()), this, SLOT(saveEstate()));

	layout = new QGridLayout(this, 5, 1, 6, 3);
	nameEdit = new QLineEdit(this, "Name Edit");
	layout->addWidget(nameEdit, 0, 0);
	connect(nameEdit, SIGNAL(returnPressed()), this, SIGNAL(somethingChanged()));

	confDlg = 0;
	oldConfDlg = 0;
	reallyOldConfDlg = 0;

	QVGroupBox *colorGroupBox = new QVGroupBox(i18n("Colors"), this);
	layout->addWidget(colorGroupBox, 2, 0);
	
	(void) new QLabel(i18n("Foreground"), colorGroupBox);
	fgButton = new KColorButton(colorGroupBox, "Foreground Button");
	connect(fgButton, SIGNAL(changed(const QColor &)), this, SIGNAL(somethingChanged()));
	(void) new QLabel(i18n("Background"), colorGroupBox);
	bgButton = new KColorButton(colorGroupBox, "Background Button");
	connect(bgButton, SIGNAL(changed(const QColor &)), this, SIGNAL(somethingChanged()));

	QHBoxLayout *typeLayout = new QHBoxLayout(layout, 6);
	QLabel *typeLabel = new QLabel(i18n("Type"), this);
	typeLayout->addWidget(typeLabel, 3, 0);
	typeCombo = new QComboBox(false, this, "Type Combo");
	typeLayout->addWidget(typeCombo, 4, 0);
	connect(typeCombo, SIGNAL(activated(int)), this, SIGNAL(somethingChanged()));

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
	kdDebug() << "starting setEstate\n";

	typeCombo->setCurrentItem(estate->type());
	nameEdit->setText(estate->name());
	if (estate->color().isValid())
		fgButton->setColor(estate->color());
	bgButton->setColor(estate->bgColor());
	this->estate = estate;

	saveEstate(true);

	kdDebug() << "ending setEstate\n";
}

ConfigEstate *EstateEdit::saveEstate(bool superficial)
{
	kdDebug() << "starting saveEstate\n";
	if (!estate)
		return 0;

	EstateType curType = (EstateType)typeCombo->currentItem();

	if (!superficial)
	{
		emit saveDialogSettings();
	
		estate->setType(curType);
		estate->setName(nameEdit->text());
		if (estate->color().isValid())
			fgButton->setColor(estate->color());
		estate->setBgColor(bgButton->color());
	}

	if (curType != Street)
	{
		fgButton->setEnabled(false);
		//fgButton->setColor(QColor("zzzzzz"));
		estate->setColor(QColor("zzzzzz"));
	}
	else
		fgButton->setEnabled(true);

	if (!superficial)
		estate->update();

	if (superficial)
		configure();

	kdDebug() << "ending saveEstate\n";
	return estate;
}

void EstateEdit::configure()
{
	kdDebug() << "-------------- ::configure()\n";
	if (oldType == typeCombo->currentItem())
	{
		kdDebug() << "returning early from configure\n";
		emit updateDialogSettings(estate);
		return;
	}

	kdDebug() << "starting configure\n";

	if (reallyOldConfDlg)
	{
		kdDebug() << "deleting reallyOldConfDlg\n";
		delete reallyOldConfDlg;
	}
	reallyOldConfDlg = oldConfDlg;

	oldConfDlg = confDlg;
	confDlg = 0;

	switch ((EstateType)typeCombo->currentItem())
	{
	case Street:
		confDlg = new StreetDlg(estate, this);
		connect(this, SIGNAL(saveDialogSettings()), static_cast<StreetDlg *>(confDlg), SLOT(slotOk()));
		connect(this, SIGNAL(updateDialogSettings(ConfigEstate *)), static_cast<StreetDlg *>(confDlg), SLOT(slotUpdate(ConfigEstate *)));
		break;
	case Tax:
		confDlg = new TaxDlg(estate, this);
		connect(this, SIGNAL(saveDialogSettings()), static_cast<TaxDlg *>(confDlg), SLOT(slotOk()));
		connect(this, SIGNAL(updateDialogSettings(ConfigEstate *)), static_cast<TaxDlg *>(confDlg), SLOT(slotUpdate(ConfigEstate *)));
		break;
	case Chance:
		confDlg = new CardView(chanceStack, this);
		//connect(this, SIGNAL(saveDialogSettings()), static_cast<CardView *>(confDlg), SLOT(slotOk()));
		//connect(this, SIGNAL(updateDialogSettings(ConfigEstate *)), static_cast<CardView *>(confDlg), SLOT(slotUpdate()));
		break;
	case CommunityChest:
		confDlg = new CardView(ccStack, this);
		//connect(this, SIGNAL(saveDialogSettings()), static_cast<CardView *>(confDlg), SLOT(slotOk()));
		//connect(this, SIGNAL(updateDialogSettings(ConfigEstate *)), static_cast<CardView *>(confDlg), SLOT(slotUpdate()));
		break;
	default:
		confDlg = new QWidget(this);
	}
	layout->addWidget(confDlg, 1, 0);
	confDlg->show();

	kdDebug() << "ending configure\n";
	oldType = (EstateType)typeCombo->currentItem();
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

TaxDlg::TaxDlg(ConfigEstate *estate, QWidget *parent, char *name)
	: QWidget(parent, name)
{
	QGridLayout *taxBox = new QGridLayout(this, 2, 2);
	taxBox->addWidget(new QLabel(i18n("Fixed tax"), this), 0, 0);
	taxBox->addWidget(tax = new QSpinBox(0, 3000, 1, this), 0, 1);
	tax->setSuffix("$");
	taxBox->addWidget(new QLabel(i18n("Percentage tax"), this), 1, 0);
	taxBox->addWidget(taxPercentage = new QSpinBox(0, 100, 1, this), 1, 1);
	taxPercentage->setSuffix("%");

	slotUpdate(estate);
}

void TaxDlg::slotOk()
{
	estate->setTax(tax->value());
	estate->setTaxPercentage(taxPercentage->value());
}

void TaxDlg::slotUpdate(ConfigEstate *estate)
{
	this->estate = estate;
	tax->setValue(estate->tax());
	taxPercentage->setValue(estate->taxPercentage());
}

/////////////////////////////////

ChooseWidget::ChooseWidget(int id, Card *card, QWidget *parent, char *name)
	: QWidget (parent, name)
{
	this->id = id;
	this->card = card;

	kdDebug() << "new choose widget, id is " << id << endl;

	QHBoxLayout *hlayout = new QHBoxLayout(this);
	typeCombo = new QComboBox(this);
	QStringList _types(i18n("Pay"));
	_types.append(i18n("Pay each player"));
	_types.append(i18n("Collect"));
	_types.append(i18n("Collect from each player"));
	_types.append(i18n("Advance to"));
	_types.append(i18n("Advance"));
	_types.append(i18n("Go back"));
	_types.append(i18n("Go to jail"));
	_types.append(i18n("Get out of jail free card"));
	_types.append(i18n("Advance to nearest utility"));
	_types.append(i18n("Advance to nearest railroad"));
	_types.append(i18n("Pay for each house"));
	_types.append(i18n("Pay for each hotel"));
	typeCombo->insertStringList(_types);
	hlayout->addWidget(typeCombo);
	connect(typeCombo, SIGNAL(activated(int)), this, SLOT(typeChanged(int)));

	hlayout->addStretch();

	value = new QSpinBox(0, 1000, 1, this);
	hlayout->addWidget(value);
	connect(value, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
}

void ChooseWidget::valueChanged(int i)
{
	(*card->values.at(id)) = i;
	value->setValue(i);
}

void ChooseWidget::typeChanged(int i)
{
	QString key = (*types.at(i));

	(*card->keys.at(id)) = key;
	typeCombo->setCurrentItem(i);

	bool boolean = (key == "jailcard" || key == "tojail" || key == "nextrr" || key == "nextutil");
	if (boolean)
	{
		value->setValue(1);
		valueChanged(1);
	}
	value->setEnabled(!boolean);

	QString suffix = "";
	QString prefix = "";
	
	// first for types are money, pay, payeach, collect, collecteach
	if (i < 4 || key == "payhouse" || key == "payhotel")
		suffix = "$";
	else if (key == "advanceto")
		prefix = i18n("Estate #");
	else if (key == "advance" || key == "goback")
		suffix = i18n("Estate(s)");

	value->setPrefix(prefix);
	value->setSuffix(suffix);
}

/////////////////////////////////

CardView::CardView(CardStack *stack, QWidget *parent, char *name) : QWidget(parent, name)
{
	this->stack = stack;

	choosies.setAutoDelete(true);

	layout = new QVBoxLayout(this, 2);
	QHBoxLayout *hlayout = new QHBoxLayout(layout);

	addButton = new KPushButton(i18n("&Add..."), this);
	connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
	hlayout->addWidget(addButton);
	hlayout->addStretch();
	renameButton = new KPushButton(i18n("&Rename..."), this);
	connect(renameButton, SIGNAL(clicked()), this, SLOT(rename()));
	hlayout->addWidget(renameButton);
	hlayout->addStretch();
	delButton = new KPushButton(i18n("&Delete"), this);
	connect(delButton, SIGNAL(clicked()), this, SLOT(del()));
	hlayout->addWidget(delButton);

	List = new QListBox(this);
	List->setMinimumHeight(150); // gets squashed vertically w/o
	layout->addWidget(List);
	connect(List, SIGNAL(highlighted(int)), this, SLOT(selected(int)));

	hlayout = new QHBoxLayout(layout);
	moreButton = new KPushButton(i18n("&More properties"), this);
	connect(moreButton, SIGNAL(clicked()), this, SLOT(more()));
	hlayout->addWidget(moreButton);
	lessButton = new KPushButton(i18n("&Less properties"), this);
	connect(lessButton, SIGNAL(clicked()), this, SLOT(less()));
	hlayout->addWidget(lessButton);

	Card *card = 0;
	for (card = stack->first(); card; card = stack->next())
	{
		List->insertItem(card->name, 0);
	}

	List->setCurrentItem(0);
}

void CardView::more()
{
	if (List->count()<= 0)
		return;

	card->keys.append("pay");
	card->values.append(1);
	ChooseWidget *newChooseWidget = new ChooseWidget(choosies.count(), card, this);

	choosies.append(newChooseWidget);
	layout->addWidget(newChooseWidget);

	newChooseWidget->show();
}

void CardView::less()
{
	if (List->count()<= 0)
		return;
	if (choosies.count() <= 0)
		return;

	choosies.removeLast();
	card->keys.pop_back();
	card->values.pop_back();
}

void CardView::add()
{
	bool ok = false;
	QString name = KLineEditDlg::getText(i18n("New card name:"), QString::null, &ok, this);
	if (ok)
		List->insertItem(name, 0);

	choosies.clear();

	Card *newCard = new Card;
	newCard->name = name;
	stack->prepend(newCard);

	List->setCurrentItem(0);

	more();
}

void CardView::rename()
{
	int curItem = List->currentItem();
	if (curItem < 0)
		return;

	bool ok = false;;
	QString name = KLineEditDlg::getText(i18n("New card name:"), List->text(curItem), &ok, this);
	if (ok)
	{
		stack->at(curItem)->name = name;
		List->changeItem(name, curItem);
	}
}

void CardView::del()
{
	int curItem = List->currentItem();
	if (curItem < 0)
		return;
	
	List->removeItem(curItem);
	stack->remove(stack->at(curItem));
	List->setCurrentItem(0);

	choosies.clear();
	more();
}

void CardView::selected(int i)
{
	card = stack->at(i);
	kdDebug() << "card is " << card->name << endl;
	unsigned int num = card->keys.count();

	choosies.clear();

	QValueList<int>::Iterator vit = card->values.begin();
	for (QStringList::Iterator it = card->keys.begin(); it != card->keys.end(); ++it)
	{
		ChooseWidget *newChooseWidget = new ChooseWidget(choosies.count(), card, this);

		choosies.append(newChooseWidget);
		layout->addWidget(newChooseWidget);

		newChooseWidget->show();
		 
		choosies.last()->typeChanged(types.findIndex(*it));
		choosies.last()->valueChanged(*vit);
		++vit;
	}

	if (num == 0)
	{
		card->values.clear();
		more();
	}
}

/////////////////////////////////

StreetDlg::StreetDlg(ConfigEstate *estate, QWidget *parent, char *name)
	: QWidget(parent, name)
{
	QVBoxLayout *bigbox = new QVBoxLayout(this);

	QVGroupBox *RentPage = new QVGroupBox(i18n("Rent"), this);
	bigbox->addWidget(RentPage);
	QWidget *topRent = new QWidget(RentPage);
	QGridLayout *rentBox = new QGridLayout(topRent, 6, 2);
	rentBox->addWidget(new QLabel(i18n("No houses"), topRent), 0, 0);
	rentBox->addWidget(new QLabel(i18n("One house"), topRent), 1, 0);
	rentBox->addWidget(new QLabel(i18n("Two houses"), topRent), 2, 0);
	rentBox->addWidget(new QLabel(i18n("Three houses"), topRent), 3, 0);
	rentBox->addWidget(new QLabel(i18n("Four houses"), topRent), 4, 0);
	rentBox->addWidget(new QLabel(i18n("One hotel"), topRent), 5, 0);
	rentBox->addWidget(houses0 = new QSpinBox(0, 3000, 1, topRent), 0, 1);
	rentBox->addWidget(houses1 = new QSpinBox(0, 3000, 1, topRent), 1, 1);
	rentBox->addWidget(houses2 = new QSpinBox(0, 3000, 1, topRent), 2, 1);
	rentBox->addWidget(houses3 = new QSpinBox(0, 3000, 1, topRent), 3, 1);
	rentBox->addWidget(houses4 = new QSpinBox(0, 3000, 1, topRent), 4, 1);
	rentBox->addWidget(houses5 = new QSpinBox(0, 3000, 1, topRent), 5, 1);
	houses0->setSuffix(i18n("$"));
	houses1->setSuffix(i18n("$"));
	houses2->setSuffix(i18n("$"));
	houses3->setSuffix(i18n("$"));
	houses4->setSuffix(i18n("$"));
	houses5->setSuffix(i18n("$"));

	QGridLayout *pricesBox = new QGridLayout(bigbox, 2, 2);
	pricesBox->addWidget(new QLabel(i18n("Price"), this), 0, 0);
	pricesBox->addWidget(price = new QSpinBox(0, 3000, 25, this), 0, 1);
	price->setSuffix(i18n("$"));
	pricesBox->addWidget(new QLabel(i18n("House price"), this), 1, 0);
	pricesBox->addWidget(housePrice = new QSpinBox(0, 3000, 25, this), 1, 1);
	housePrice->setSuffix(i18n("$"));

	QHBoxLayout *groupLayout = new QHBoxLayout(bigbox, 6);
	QLabel *groupLabel = new QLabel(i18n("Group"), this);
	groupLayout->addWidget(groupLabel);
	groupCombo = new QComboBox(false, this, "Group Combo");
	QStringList groups(i18n("None"));
	for (int i = 0; i <= 20; i++)
	{
		groups.append(QString::number(i));
	}
	groupCombo->insertStringList(groups);
	groupLayout->addWidget(groupCombo);

	slotUpdate(estate);
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
}

void StreetDlg::slotUpdate(ConfigEstate *estate)
{
	this->estate = estate;

	houses0->setValue(estate->rent(0));
	houses1->setValue(estate->rent(1));
	houses2->setValue(estate->rent(2));
	houses3->setValue(estate->rent(3));
	houses4->setValue(estate->rent(4));
	houses5->setValue(estate->rent(5));

	price->setValue(estate->price());
	housePrice->setValue(estate->housePrice());

	groupCombo->setCurrentItem(estate->group() + 1);
}

#include "editor.moc"
