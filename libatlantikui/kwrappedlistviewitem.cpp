// Copyright (c) 2004 Rob Kaper <rob@unixcode.org>. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS `AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

#include <qheader.h>
#include <qstring.h>

#include <kglobalsettings.h>
#include <klistview.h>
#include <kwordwrap.h>

#include "kwrappedlistviewitem.h"

KWrappedListViewItem::KWrappedListViewItem( QListView *parent, QString text, QString t2 )
: QObject(), KListViewItem( parent )
{
	init( parent, text, t2 );
}

KWrappedListViewItem::KWrappedListViewItem( QListView *parent, QListViewItem *after, QString text, QString t2 )
: QObject(), KListViewItem( parent, after )
{
	init( parent, text, t2 );
}

void KWrappedListViewItem::setup()
{
	widthChanged();
}

/*
int KWrappedListViewItem::width( const QFontMetrics&, const QListView*, int ) const
{
	return m_wrap->boundingRect().width();
}
*/

void KWrappedListViewItem::wrapColumn( int c )
{
	if ( c != m_wrapColumn )
		return;

	QListView *lv = listView();
	if ( !lv )
		return;

	QFont font = QFont( KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize(), QFont::Normal );
	QFontMetrics fm = QFontMetrics( font );

	int wrapWidth = lv->width();
	for ( int i = 0 ; i < m_wrapColumn ; i++ )
		wrapWidth -= ( width(fm, lv, i) + lv->itemMargin() );

	if ( pixmap(c) )
		wrapWidth -= ( pixmap( c )->width() + lv->itemMargin() );

	QScrollBar *scrollBar = lv->verticalScrollBar();
	if ( !scrollBar->isHidden() )
		wrapWidth -= scrollBar->width();

	QRect rect = QRect( 0, 0, wrapWidth - 20, -1 );

	KWordWrap *wrap = KWordWrap::formatText( fm, rect, 0, m_origText );
	setText( c, wrap->wrappedString() );

	int lc = text(c).contains( QChar( '\n' ) ) + 1;
	setHeight( wrap->boundingRect().height() + lc*lv->itemMargin() );

	widthChanged( c );

	delete wrap;
}

void KWrappedListViewItem::init( QListView *parent, QString text, QString t2 )
{
	m_wrapColumn = 0;
	setMultiLinesEnabled( true );
	parent->setResizeMode( QListView::LastColumn );

	m_origText = text;

	if ( !t2.isNull() )
	{
		setText( 0, text );
		m_origText = t2;
		m_wrapColumn = 1;
	}
	else
		m_origText = text;

	wrapColumn( m_wrapColumn );
		
	connect( parent->header(), SIGNAL(sizeChange(int, int, int)), this, SLOT(wrapColumn(int)));
}

#include "kwrappedlistviewitem.moc"
