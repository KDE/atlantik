#ifndef ATLANTIK_DISPLAY_WIDGET_H
#define  ATLANTIK_DISPLAY_WIDGET_H

#include <qwidget.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qtextedit.h>

class BoardDisplay : public QWidget
{
Q_OBJECT

public:
	BoardDisplay(const QString type, const QString description, QWidget *parent, const char *name=0);

	void initPage();
	bool validateNext();

	private slots:
		void slotClicked();

	private:
		QTextEdit *m_label;
		QVBoxLayout *m_mainLayout;
		QVGroupBox *m_playerGroupBox;
};

#endif
