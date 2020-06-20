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

#ifndef ATLANTIK_ESTATEDETAILSBASE_H
#define ATLANTIK_ESTATEDETAILSBASE_H

#include <QWidget>

class QPixmap;

class Estate;

class EstateDetailsBase : public QWidget
{
Q_OBJECT

public:
	EstateDetailsBase(Estate *estate, QWidget *parent);
	~EstateDetailsBase();
	Estate *estate() const { return m_estate; }

	void setEstate(Estate *estate);

protected:
	void paintEvent(QPaintEvent *) override;
	void resizeEvent(QResizeEvent *) override;

	QWidget *widget() { return m_widget; }

private:
	Estate *m_estate;
	QWidget *m_widget;
	QPixmap *m_pixmap;
	QPixmap *m_quartzBlocks;
	bool b_recreate, m_recreateQuartz;
};

#endif
