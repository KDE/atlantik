#ifndef __KMONOP_NEWGAMEDLG_H__
#define __KMONOP_NEWGAMEDLG_H__

#include <qlistview.h>
#include <qstring.h>
#include <qradiobutton.h>

#include <kwizard.h>
#include <kdialogbase.h>

#include "network.h"

class SelectServer : public QWidget
{
Q_OBJECT

	public:
		SelectServer(QWidget *parent, const char *name=0);
		void initPage();
		bool validateNext();
		QString hostToConnect() const;

//	public slots:
//		void slotFetchedServerList(QDomNode);

	signals:
		void statusChanged();

	private:
		QListView *list;
		QLabel *status_label;
};

class SelectGame : public QWidget
{
Q_OBJECT

	public:
		SelectGame(QWidget *parent, const char *name=0);
		void initPage();
		void setGameHost(const QString &);
		void validateButtons();
		bool validateNext();
		QString gameToJoin() const;

	public slots:
		void slotConnectionError(int);
		void slotConnected();

		void slotGamelistUpdate(QString);
		void slotGamelistEndUpdate(QString);
		void slotGamelistAdd(QString, QString);
		void slotGamelistEdit(QString, QString);
		void slotGamelistDel(QString);

		void slotInitPage();

	signals:
		void statusChanged();

	private:
		QRadioButton *bnew, *bjoin;
		QListView *list;
		QLabel *status_label;
		QString gameHost;
};

class ConfigureGame : public QWidget
{
Q_OBJECT

	public:
		ConfigureGame(QWidget *parent, const char *name=0);
		void initPage();
		void setGameId(const QString &);
		bool validateNext();

	public slots:
		void slotPlayerlistUpdate(QString);
		void slotPlayerlistEndUpdate(QString);
		void slotPlayerlistAdd(QString, QString, QString);
		void slotPlayerlistEdit(QString, QString, QString);
		void slotPlayerlistDel(QString);

	signals:
		void statusChanged();

	private:
		QListView *list;
		QLabel *status_label;
		QString game_id;
};

class NewGameWizard : public KWizard
{
Q_OBJECT

	public:
		NewGameWizard(QWidget *parent, const char *name=0, bool modal=true, WFlags f=0);
		~NewGameWizard(void);

	public slots:
		void slotValidateNext();
		void slotInit(const QString &);

	private:
		QListView *list;
		SelectServer *select_server;
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
