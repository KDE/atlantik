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
         
#ifndef KWRAPPEDLISTVIEWITEM_H
#define KWRAPPEDLISTVIEWITEM_H

#include <k3listview.h>


class KWrappedListViewItem : public QObject, public K3ListViewItem
{
Q_OBJECT

public:
	KWrappedListViewItem( Q3ListView *parent, const QString& text, const QString &t2=QString::null );
	KWrappedListViewItem( Q3ListView *parent, Q3ListViewItem *after, const QString& text, const QString& t2=QString::null );
	void setup();
//	int width(const QFontMetrics& fm, const QListView* lv, int c) const;

private slots:
	void wrapColumn( int c );

private:
	void init( Q3ListView *parent, const QString& text, const QString& t2=QString::null );
	QString m_origText;
	int m_wrapColumn;
};

#endif
