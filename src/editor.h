#ifndef EDITOR_H
#define EDITOR_H

#include <kmainwindow.h>
#include <qstring.h>
#include <qcolor.h>
#include "board.h"
#include "estate.h"

class QCheckBox;
class QComboBox;
class KColorButton;
class QLineEdit;

class ConfigEstate : public Estate
{
	public:
	ConfigEstate(int estateId);
	void setType(const int type);
	int type() {return m_type; };

	private:
	int m_type;
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

	private:
	KColorButton *fgButton;
	KColorButton *bgButton;
	QComboBox *typeCombo;
	QLineEdit *nameEdit;
	QWidget *centerWidget;

	ConfigEstate *estate;

	QColor fg;
	QColor bg;
};

#endif
