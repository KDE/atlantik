#ifndef ATLANTIK_AUCTION_WIDGET_H
#define ATLANTIK_AUCTION_WIDGET_H

#include <qwidget.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qpushbutton.h>
#include <qtextedit.h>

#include <klistview.h>

class AuctionWidget : public QWidget
{
Q_OBJECT

public:
	AuctionWidget(QWidget *parent, const char *name=0);

private:
	QTextEdit *m_label;
	QVBoxLayout *m_mainLayout;
	QVGroupBox *m_playerGroupBox;
};

#endif
