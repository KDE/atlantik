#ifndef ATLANTIK_CONFIGDLG_H
#define ATLANTIK_CONFIGDLG_H

#include <qwidget.h>
#include <qcheckbox.h>
#include <qlineedit.h>

#include <kdialogbase.h>

class Atlantik;
class ConfigDialog;

struct AtlantikConfig;

class ConfigPlayer : public QWidget
{
Q_OBJECT

public:
	ConfigPlayer(ConfigDialog *configDialog, QWidget *parent, const char *name=0);

	QString playerName();

private:
	void reset();

	ConfigDialog *m_configDialog;
	QLineEdit *m_playerName;
};

class ConfigBoard : public QWidget
{
Q_OBJECT

public:
	ConfigBoard(ConfigDialog *configDialog, QWidget *parent, const char *name=0);

	bool indicateUnowned();
	bool highliteUnowned();
	bool darkenMortgaged();
	bool animateToken();
	bool quartzEffects();

private:
	void reset();

	ConfigDialog *m_configDialog;
	QCheckBox *m_indicateUnowned, *m_highliteUnowned, *m_darkenMortgaged, *m_animateToken, *m_quartzEffects;
};

class ConfigDialog : public KDialogBase
{
Q_OBJECT

public:
	ConfigDialog(Atlantik *parent, const char *name=0);

	bool indicateUnowned();
	bool highliteUnowned();
	bool darkenMortgaged();
	bool animateToken();
	bool quartzEffects();
	AtlantikConfig config();
	QString playerName();

private:
	Atlantik *m_parent;
	QFrame *p_p13n, *p_board;
	ConfigPlayer *configPlayer;
	ConfigBoard *configBoard;
};

#endif
