#ifndef EDITOR_H
#define EDITOR_H

#include <kdialogbase.h>
#include <kmainwindow.h>
#include <qstring.h>
#include <qcolor.h>
#include "board.h"
#include "estate.h"

class QCheckBox;
class QPushButton;
class QComboBox;
class QSpinBox;
class KColorButton;
class QLineEdit;

// following enum taken from monopd/estate.h, and numbers all decreased by one
enum EstateType { Street=0, RR=1, Utility=2, CommunityChest=3, Chance=4, FreeParking=5, ToJail=6, Tax=7, Airport=8, Jail=9, Go=10, Other=11 };

class ConfigEstate : public Estate
{
	public:
	ConfigEstate(int estateId);
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

class EstateEdit : public AtlantikBoard
{
	Q_OBJECT

	public:
	EstateEdit(QWidget *parent = 0, const char *name = 0);
	
	public slots:
	void setEstate(ConfigEstate *);
	ConfigEstate *saveEstate();

	signals:
	void somethingChanged();
	void updateViews();
	void modified();
	
	private slots:
	void configure();

	private:
	KColorButton *fgButton;
	KColorButton *bgButton;
	QPushButton *configureButton;
	QComboBox *typeCombo;
	QLineEdit *nameEdit;
	QWidget *centerWidget;

	ConfigEstate *estate;

	QColor fg;
	QColor bg;
};

class TaxDlg : public KDialogBase
{
	Q_OBJECT

	public:
	TaxDlg(ConfigEstate *, QWidget *parent = 0, char *name = 0, bool modal = 0);

	protected slots:
	void slotOk();

	private:
	ConfigEstate *estate;
	QSpinBox *tax;
	QSpinBox *taxPercentage;
};

class StreetDlg : public KDialogBase
{
	Q_OBJECT

	public:
	StreetDlg(ConfigEstate *, QWidget *parent = 0, char *name = 0, bool modal = 0);

	protected slots:
	void slotOk();

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
	QComboBox *groupCombo;
};

#endif
