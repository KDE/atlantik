#ifndef EDITOR_H
#define EDITOR_H

#include <kmainwindow.h>
#include <qstring.h>

class QCheckBox;
class QComboBox;
class KDualColorButton;
class QLineEdit;

class EstateEdit : public QWidget
{
	Q_OBJECT

	public:
	EstateEdit(QWidget *parent = 0, const char *name = 0);

	private:
	KDualColorButton *fgbgButton;
	QComboBox *typeCombo;
	QLineEdit *nameEdit;
};

#endif
