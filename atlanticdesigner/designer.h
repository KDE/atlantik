#ifndef DESIGNER_H
#define DESIGNER_H

#include <kmainwindow.h>
#include <qstring.h>

#include "editor.h"

class EstateEdit;
class QCloseEvent;
class KListAction;

class TopLevel : public KMainWindow
{
	Q_OBJECT

	public:
	TopLevel(QWidget *parent = 0, const char *name = 0);

	protected:
	void closeEvent(QCloseEvent *);

	private slots:
	void open();
	void openNew();
	void save();
	void changeEstate(int);

	private:
	EstateEdit *editor;
	QValueList<Estate> estates;
	KListAction *estateAct;

	QString filename;

	int max;
	QStringList types;
};

#endif
