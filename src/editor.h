#ifndef ATLANTIK_EDITOR_H
#define ATLANTIK_EDITOR_H

#include <kmainwindow.h>
#include <qstring.h>
#include <qcolor.h>
#include <qstringlist.h>
#include "estate.h"

class QCheckBox;
class KComboBox;
class QSpinBox;
class KColorButton;
class QLineEdit;
class KPushButton;

// following enum taken from monopd/estate.h, and numbers all decreased by one
enum EstateType { Street=0, RR=1, Utility=2, CommunityChest=3, Chance=4, FreeParking=5, ToJail=6, Tax=7, Airport=8, Jail=9, Go=10, Other=11 };

class ConfigEstate : public Estate
{
	public:
	ConfigEstate(int estateId);
	void setEstateId(const int estateId);
	int type() {return m_type; };
	void setType(const int type);
	int group() {return m_group; };
	void setGroup(const int group);
	int price() { return m_price; }
	void setPrice(int);
	int housePrice() { return m_housePrice; }
	void setHousePrice(const int);
	int rent(int _h) { return m_rent[_h]; }
	void setRent(const int, const int);
	int tax() { return m_tax; }
	void setTax(const int);
	int taxPercentage() {return m_taxPercentage; }
	void setTaxPercentage(const int);

	void setChanged(bool);

	private:
	int m_type;
	int m_group;
	int m_rent[6];
	int m_price;
	int m_housePrice;
	int m_tax;
	int m_taxPercentage;
};
typedef QPtrList<ConfigEstate> EstateList;

struct Card
{
	Card() { name = QString::null; }
	QString name;
	QStringList keys;
	QValueList<int> values;
};
typedef QPtrList<Card> CardStack;

class EstateEdit : public QWidget
{
	Q_OBJECT

	public:
	EstateEdit(EstateList *, CardStack *, CardStack *, QWidget *parent = 0, const char *name = 0);
	ConfigEstate *theEstate() { return estate; }
	bool upArrow();
	bool downArrow();
	bool leftArrow();
	bool rightArrow();
	void aboutToDie();
	void setReady(bool);
	
	public slots:
	void setEstate(ConfigEstate *);
	ConfigEstate *saveEstate(bool superficial = false);

	signals:
	void somethingChanged();
	void modified();
	void saveDialogSettings();
	void updateDialogSettings(ConfigEstate *);
	
	private slots:
	void configure();

	private:
	KColorButton *fgButton;
	KColorButton *bgButton;
	KComboBox *typeCombo;
	QLineEdit *nameEdit;
	QWidget *centerWidget;
	QGridLayout *layout;

	EstateList *estates;

	ConfigEstate *estate;
	CardStack *chanceStack;
	CardStack *ccStack;

	QWidget *confDlg;
	QWidget *oldConfDlg;
	QWidget *reallyOldConfDlg;

	QColor fg;
	QColor bg;

	EstateType oldType;

	bool ready;
};

class ChooseWidget : public QWidget
{
	Q_OBJECT
	
	public:
	ChooseWidget(EstateList *, int id, Card *, QWidget *parent = 0, char *name = 0);

	public slots:
	void typeChanged(int);
	void valueChanged(int);
	void estateChanged(int);

	private:
	Card *card;
	KComboBox *typeCombo;
	QSpinBox *value;
	KComboBox *estate;
	bool number;
	bool prevNumber;
	bool init;
	QHBoxLayout *hlayout;

	EstateList *estates;

	int id;
};

class CardView : public QWidget
{
	Q_OBJECT

	public:
	CardView(EstateList *, CardStack *, QWidget *parent = 0, char *name = 0);

	private slots:
	void selected(int);
	void add();
	void del();
	void rename();
	void more();
	void less();
	void updateButtonsEnabled();

	private:
	QListBox *List;
	KPushButton *addButton;
	KPushButton *renameButton;
	KPushButton *delButton;
	KPushButton *moreButton;
	KPushButton *lessButton;

	EstateList *estates;

	QVBoxLayout *layout;

	Card *card;

	CardStack *stack;
	QPtrList<ChooseWidget> choosies;
};

class TaxDlg : public QWidget
{
	Q_OBJECT

	public:
	TaxDlg(ConfigEstate *, QWidget *parent = 0, char *name = 0);

	public slots:
	void slotOk();
	void slotUpdate(ConfigEstate *);

	private:
	ConfigEstate *estate;
	QSpinBox *tax;
	QSpinBox *taxPercentage;
};

class StreetDlg : public QWidget
{
	Q_OBJECT

	public:
	StreetDlg(ConfigEstate *, QWidget *parent = 0, char *name = 0);

	public slots:
	void slotOk();
	void slotUpdate(ConfigEstate *);

	private:
	ConfigEstate *estate;
	QSpinBox *houses0;
	QSpinBox *houses1;
	QSpinBox *houses2;
	QSpinBox *houses3;
	QSpinBox *houses4;
	QSpinBox *houses5;
	QSpinBox *price;
	QSpinBox *housePrice;
	KComboBox *groupCombo;
};

#endif
