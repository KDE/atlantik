#ifndef __KMONOP_CONFIGDLG_H__
#define __KMONOP_CONFIGDLG_H__

#include <qwidget.h>
#include <qcheckbox.h>
#include <qlineedit.h>

#include <kdialogbase.h>

class ConfigPlayer : public QWidget
{
	Q_OBJECT

friend class ConfigDialog;

	public:
		ConfigPlayer(QWidget *parent, const char *name=0);

	private:
		void reset();

		QLineEdit *_playerName;
};

class ConfigBoard : public QWidget
{
	Q_OBJECT

friend class ConfigDialog;

	public:
		ConfigBoard(QWidget *parent, const char *name=0);

	private:
		void reset();

		QCheckBox *_indicateUnowned, *_animateToken;
};

class ConfigDialog : public KDialogBase
{
	Q_OBJECT

	public:
		ConfigDialog(QWidget *parent, const char *name=0);
		bool indicateUnowned();
		bool animateToken();
		QString playerName();

	private:
		QFrame *p_p13n, *p_board;
		ConfigPlayer *configPlayer;
		ConfigBoard *configBoard;
};

#endif
