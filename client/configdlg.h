#ifndef ATLANTIK_CONFIGDLG_H
#define ATLANTIK_CONFIGDLG_H

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

		QLineEdit *m_playerName;
};

class ConfigBoard : public QWidget
{
	Q_OBJECT

friend class ConfigDialog;

	public:
		ConfigBoard(QWidget *parent, const char *name=0);

	private:
		void reset();

		QCheckBox *m_indicateUnowned, *m_highliteUnowned, *m_darkenMortgaged, *m_animateToken, *m_quartzEffects;
};

class ConfigDialog : public KDialogBase
{
	Q_OBJECT

	public:
		ConfigDialog(QWidget *parent, const char *name=0);

		bool indicateUnowned();
		bool highliteUnowned();
		bool darkenMortgaged();
		bool animateToken();
		bool quartzEffects();

		QString playerName();

	private:
		QFrame *p_p13n, *p_board;
		ConfigPlayer *configPlayer;
		ConfigBoard *configBoard;
};

#endif
