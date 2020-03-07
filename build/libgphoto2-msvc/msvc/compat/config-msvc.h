/** \file
* \brief configuration for msvc compiler
*
* \author Copyright 2016 Michael Fink <vividos@users.sf.net>
*
* \note
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* \note
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* \note
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301  USA
*/
#pragma once

// includes and defines for C functions
#include <string.h>
#define strdup _strdup

// for integer types, such as uint32_t
#include <stdint.h>

#include <io.h>
#define close _close
#define dup _dup
#define lseek _lseek
#define read _read
#define write _write
#define ftruncate _chsize

#include <stdio.h>
#define unlink _unlink
#define fileno _fileno

// gcc macro
#ifndef __func__
#define __func__ __FUNCTION__
#endif

// posix extension; signed size_t
typedef long ssize_t;

// inline for C
#define inline __inline
