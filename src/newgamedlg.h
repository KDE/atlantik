#ifndef __KMONOP_NEWGAMEDLG_H__
#define __KMONOP_NEWGAMEDLG_H__

#include <qlistview.h>
#include <qpushbutton.h>
#include <qstring.h>

#include <kwizard.h>
#include <kdialogbase.h>

class SelectGame : public QWidget
{
	public:
		SelectGame(QWidget *parent, const char *name=0);
		void initPage();
};

class NewGameWizard : public KWizard
{
Q_OBJECT

	public:
		NewGameWizard(QWidget *parent, const char *name=0, bool modal=true, WFlags f=0);
		~NewGameWizard(void);

	public slots:
		void slotListClick(QListViewItem *);
		void slotInit(const QString &);
		void slotConnected();	

	private:
		QListView *list;
		QWidget *select_server;
		SelectGame *select_game;
};

class Server
{
	public:
		Server(const QString& _host, int _port)
		{
			h = _host;
			p = _port;
		}

		QString host() const
		{
			return h;
		}

		int port() const
		{
			return p;
		}
	
	private:
		QString h;
		int p;
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
