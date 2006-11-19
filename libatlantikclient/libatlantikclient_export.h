// Copyright (c) 2004 Dirk Mueller <mueller@kde.org>

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

#ifndef LIBATLANTIKCLIENT_EXPORT_H
#define LIBATLANTIKCLIENT_EXPORT_H

#include <kdemacros.h>

#ifdef Q_WS_WIN

#ifndef LIBANTLANTIKCLIENT_EXPORT
# ifdef MAKE_ATLANTIKCLIENT_LIB
#  define LIBATLANTIKCLIENT_EXPORT KDE_EXPORT
# else
#  define LIBATLANTIKCLIENT_EXPORT KDE_IMPORT
# endif
#endif

#else
#define LIBATLANTIKCLIENT_EXPORT KDE_EXPORT
#endif

#endif
