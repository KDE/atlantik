// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <qlayout.h>
#include <qlabel.h>

#include <kdialog.h>
#include <kdiroperator.h>
#include <kfileiconview.h>
#include <kfileview.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include "tokenwidget.moc"

TokenWidget::TokenWidget(QWidget *parent, const char *name) : QWidget(parent, name)
{
	setCaption(i18n("Select Token"));

	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint());
	Q_CHECK_PTR(m_mainLayout);

	QLabel *label = new QLabel(i18n("Select a token:"), this);
	m_mainLayout->addWidget(label);

	KURL startDir(locate("appdata", "themes/default/tokens/"));

	KFileIconView *view = new KFileIconView( this , "fileIconView");
	m_mainLayout->addWidget(view);
	view->setMinimumSize(250, 250);

	KDirOperator *kdirop = new KDirOperator( startDir, this );
	m_mainLayout->addWidget(kdirop);

	connect( kdirop, SIGNAL( fileSelected( const KFileItem * )), SLOT( slotIconSelected( const KFileItem * )));
	connect( kdirop, SIGNAL( finishedLoading() ), view, SLOT( showPreviews() ));

	kdirop->setView( view );
}

void TokenWidget::slotIconSelected(const KFileItem *item)
{
	emit iconSelected(item->name());
}
