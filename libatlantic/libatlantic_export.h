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

#ifndef LIBATLANTIC_EXPORT_H
#define LIBATLANTIC_EXPORT_H

/* needed for KDE_EXPORT macros */
#include <kdemacros.h>

/* needed, because e.g. Q_OS_UNIX is so frequently used */
#include <qglobal.h>

#ifdef MAKE_ATLANTIC_LIB
# define LIBATLANTIC_EXPORT KDE_EXPORT
#else
# ifdef Q_OS_WIN
#  define LIBATLANTIC_EXPORT KDE_IMPORT
# else
#  define LIBATLANTIC_EXPORT KDE_EXPORT
# endif
#endif

#endif
