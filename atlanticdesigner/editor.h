#ifndef EDITOR_H
#define EDITOR_H

#include <kmainwindow.h>
#include <qstring.h>
#include <qcolor.h>

class QCheckBox;
class QComboBox;
class KColorButton;
class QLineEdit;

struct Estate
{
	QString name;
	QColor fg;
	QColor bg;
	int type;
	int num;
};

class EstateEdit : public QWidget
{
	Q_OBJECT

	public:
	EstateEdit(QWidget *parent = 0, const char *name = 0);
	Estate estate();
	
	public slots:
	void setEstate(Estate);

	private:
	KColorButton *fgButton;
	KColorButton *bgButton;
	QComboBox *typeCombo;
	QLineEdit *nameEdit;

	QColor fg;
	QColor bg;

	int num;
};

#endif
