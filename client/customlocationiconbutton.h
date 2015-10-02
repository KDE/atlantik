// Copyright (c) 2002-2004 Rob Kaper <cap@capsi.com>
// Copyright (c) 2015 Pino Toscano <pino@kde.org>
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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef ATLANTIK_CUSTOMLOCATIONICONBUTTON_H
#define ATLANTIK_CUSTOMLOCATIONICONBUTTON_H

#include <QPushButton>

class CustomLocationIconButton : public QPushButton
{
Q_OBJECT

Q_PROPERTY(QString image READ image WRITE setImage NOTIFY imageChanged)

public:
	CustomLocationIconButton(QWidget *parent = 0);
	~CustomLocationIconButton();

	void setLocation(const QByteArray &resource, const QString &path);

	void setImage(const QString &image);
	QString image() const;

signals:
	void imageChanged(const QString &image);

private slots:
	void slotChooseImage();

private:
	bool checkValid() const;

	QByteArray m_resource;
	QString m_path;
	QString m_image;
};

#endif