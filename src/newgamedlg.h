#ifndef __KMONOP_NEWGAMEDLG_H__
#define __KMONOP_NEWGAMEDLG_H__

#include <qlistview.h>
#include <qpushbutton.h>
#include <qstring.h>

#include <kwizard.h>
#include <kdialogbase.h>

class NewGameWizard : public KWizard
{
Q_OBJECT

	public:
		NewGameWizard(QWidget *parent, const char *name=0, bool modal=true, WFlags f=0);
		~NewGameWizard(void);

	public slots:
		void slotListClick(QListViewItem *);
		void slotInit(const QString &);
//		void slotConnect();
//		void slotCancel();

	private:
		QListView *list;
		QWidget *select_server, *select_game;
};

class SelectGame : public QWidget
{
	public:
		SelectGame(QWidget *parent, const char *name=0);
};

class NewGameDialog : public KDialog
{
Q_OBJECT

	public:
		NewGameDialog(QWidget *parent, const char *name=0, bool modal=true);
		~NewGameDialog(void);

	public slots:
		void slotConnect();
		void slotCancel();

	private:
		QListView *list;
		QPushButton *bconnect, *bcancel;
};

#endif
