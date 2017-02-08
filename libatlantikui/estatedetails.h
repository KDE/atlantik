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
#include <QVector>
#include <QSignalMapper>

class QPixmap;
class QHBoxLayout;

class QListWidget;
class QPushButton;

class Estate;

class EstateDetails : public QWidget
{
Q_OBJECT

public:
	EstateDetails(Estate *estate, const QString &text, QWidget *parent);
	~EstateDetails();
	Estate *estate() const { return m_estate; }

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
	QListWidget *m_infoListView;
	QPushButton *m_closeButton;
	bool b_recreate, m_recreateQuartz;
	QHBoxLayout *m_buttonBox;
	QSignalMapper m_buttonCommandMapper;
	QVector<QPushButton *> m_buttons;
};

#endif
