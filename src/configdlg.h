#ifndef __KMONOP_CONFIGDLG_H__
#define __KMONOP_CONFIGDLG_H__

#include <qwidget.h>
#include <qcheckbox.h>

#include <kdialogbase.h>

struct ConfigOptions
{
	bool indicateUnowned;
	bool animateToken;
};

class ConfigBoard : public QWidget
{
	Q_OBJECT

	public:
		ConfigBoard(QWidget *parent, const char *name=0);
//		ConfigOptions options();

	private:
		void reset();

		ConfigOptions _options;

		QCheckBox *indicateUnowned, *animateToken;
};

class ConfigDialog : public KDialogBase
{
	Q_OBJECT

	public:
		ConfigDialog(QWidget *parent, const char *name=0);
//		ConfigOptions options();

	private:
		ConfigOptions _options;
};

#endif
