#ifndef DESIGNER_H
#define DESIGNER_H

#include <kmainwindow.h>
#include <qstring.h>
#include <qptrlist.h>

#include "editor.h"

class EstateEdit;
class QCloseEvent;
class KListAction;
class Estate;
class Player;

class AtlanticDesigner : public KMainWindow
{
	Q_OBJECT

	public:
	AtlanticDesigner(QWidget *parent = 0, const char *name = 0);
	~AtlanticDesigner();

	protected:
	void closeEvent(QCloseEvent *);

	public slots:
	void modified();

	private slots:
	void open();
	void openNew();
	void save();
	void changeEstate(int);
	void changeEstate(Estate *);
	void movePlayer(int);

	void smaller();
	void larger();

	private:
	EstateEdit *editor;
	QPtrList<ConfigEstate> estates;
	KListAction *estateAct;

	QString filename;

	void doCaption(bool);

	int max;
	QStringList types;

	Player *player;

	bool isMod;
};

#endif
