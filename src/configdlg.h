#ifndef __KMONOP_CONFIGDLG_H__
#define __KMONOP_CONFIGDLG_H__

#include <qwidget.h>

#include <kdialogbase.h>

class ConfigBoard : public QWidget
{
	Q_OBJECT

	public:
		ConfigBoard(QWidget *parent, const char *name=0);
};

class ConfigDialog : public KDialogBase
{
	Q_OBJECT

	public:
		ConfigDialog(QWidget *parent, const char *name=0);
};

#endif
