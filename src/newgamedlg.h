#ifndef __KMONOP_NEWGAMEDLG_H__
#define __KMONOP_NEWGAMEDLG_H__

#include <qlistview.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qdom.h>
#include <qradiobutton.h>

#include <kwizard.h>
#include <kdialogbase.h>

#include "network.h"

class SelectGame : public QWidget
{
Q_OBJECT

	public:
		SelectGame(GameNetwork *, QWidget *parent, const char *name=0);
		void initPage();
		bool validateNext();
		QString gameToJoin() const;

	public slots:
		void slotConnected();	
		void slotFetchedGameList(QDomNode);

	private:
		QRadioButton *bnew, *bjoin;
		QListView *list;
		QLabel *status_label;
		GameNetwork *netw;
};

class ConfigureGame : public QWidget
{
Q_OBJECT

	public:
		ConfigureGame(GameNetwork *, QWidget *parent, const char *name=0);
		void initPage();
		void setGameId(const QString &);

	public slots:
		void slotFetchedPlayerList(QDomNode);

	private:
		QListView *list;
		QLabel *status_label;
		GameNetwork *netw;
		QString game_id;
};

class NewGameWizard : public KWizard
{
Q_OBJECT

	public:
		NewGameWizard(QWidget *parent, const char *name=0, bool modal=true, WFlags f=0);
		~NewGameWizard(void);

	public slots:
		void slotListClick();
		void slotValidateNext();
		void slotInit(const QString &);

	protected:
		GameNetwork *netw;

	private:
		QListView *list;
		QWidget *select_server;
		SelectGame *select_game;
		ConfigureGame *configure_game;
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

#endif
