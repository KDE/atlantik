// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef ATLANTIK_ESTATEDETAILS_H
#define ATLANTIK_ESTATEDETAILS_H

#include <QWidget>
//Added by qt3to4:
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QVector>
#include <QSignalMapper>

class QPixmap;
class QHBoxLayout;
class QVBoxLayout;
class QVGroupBox;

class K3ListView;
class KPushButton;

class Estate;

class EstateDetails : public QWidget
{
Q_OBJECT

public:
	EstateDetails(Estate *estate, const QString &text, QWidget *parent);
	~EstateDetails();
	Estate *estate() { return m_estate; }

	void addDetails();
	void addButton(const QString &command, const QString &caption, bool enabled);
	void addCloseButton();
	void setEstate(Estate *estate);
	void setText(const QString &text);
	void appendText(const QString &text);
	void clearButtons();

protected:
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);

signals:
	void buttonCommand(const QString&);
	void buttonClose();

private:
	Estate *m_estate;
	QPixmap *m_pixmap;
	QPixmap *m_quartzBlocks;
	K3ListView *m_infoListView;
	KPushButton *m_closeButton;
	bool b_recreate, m_recreateQuartz;
	QVBoxLayout *m_mainLayout;
	QHBoxLayout *m_buttonBox;
	QVGroupBox *m_textGroupBox;
	QSignalMapper m_buttonCommandMapper;
	QVector<KPushButton *> m_buttons;
};

#endif
