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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef ATLANTIK_ESTATEDETAILS_H
#define ATLANTIK_ESTATEDETAILS_H

#include <qwidget.h>

class QPixmap;

class KPixmap;

class Player;
class Estate;

class EstateDetails : public QWidget
{
Q_OBJECT

	public:
		EstateDetails(Estate *estate, QWidget *parent, const char *name = 0);
		Estate *estate() { return m_estate; }

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);

	private:
		Estate *m_estate;
		QPixmap *m_pixmap;
		KPixmap *m_quartzBlocks;
		bool b_recreate, m_recreateQuartz;
};

#endif
