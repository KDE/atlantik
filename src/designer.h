#ifndef ATLANTK_DESIGNER_H
#define ATLANTK_DESIGNER_H

#include <kmainwindow.h>
#include <qstring.h>
#include <qptrlist.h>
#include <qvaluelist.h>
#include <qguardedptr.h>

#include "editor.h"
#include "board.h"

class EstateEdit;
class QCloseEvent;
class KListAction;
class KRecentFilesAction;
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
	void openRecent(const KURL &);
	void openNew();
	void save();
	void saveAs();
	void copy();
	void paste();
	void changeEstate(int);
	void changeEstate(Estate *);
	void movePlayer(Estate *);
	void setPlayerAtBeginning();

	void up();
	void down();
	void left();
	void right();

	void smaller();
	void larger();

	void updateJumpMenu();

	private:
	void openFile(const QString &);
	bool warnClose();
	void initBoard();
	bool firstBoard;

	QGuardedPtr<EstateEdit> editor;
	QGuardedPtr<QVBoxLayout> layout;
	QGuardedPtr<AtlantikBoard> board;
	EstateList estates;
	KListAction *estateAct;
	KRecentFilesAction *recentAct;

	CardStack chanceStack;
	CardStack ccStack;

	QString filename;

	ConfigEstate *copiedEstate;

	void doCaption(bool);

	int max;
	QStringList types;

	Player *m_player;

	bool isMod;
};

#endif
