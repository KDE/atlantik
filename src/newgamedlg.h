#ifndef __KMONOP_NEWGAMEDLG_H__
#define __KMONOP_NEWGAMEDLG_H__

#include <qlistview.h>
#include <qpushbutton.h>
#include <qstring.h>

#include <kdialogbase.h>

class NewGameDialog : public KDialog
{
Q_OBJECT

	public:
		NewGameDialog(QWidget *parent, const char *name, bool modal);
		~NewGameDialog(void);

	public slots:
		void slotConnect();
		void slotCancel();

	private:
		QListView *list;
		QPushButton *bconnect, *bcancel;
};

#endif
