/** \file
* \brief configuration for msvc compiler, project libgphoto2
*
* \author Copyright 2016-2019 Michael Fink <vividos@users.sf.net>
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

/* Define if building universal (internal helper macro) */
#undef AC_APPLE_UNIVERSAL_BUILD

/* Whether we have flex and bison for GP2DDB */
#undef ENABLED_GP2DDB

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#undef ENABLE_NLS

/* The gettext domain we're using */
#undef GETTEXT_PACKAGE

/* Define if there is asm .symver support. */
#undef HAVE_ASM_SYMVERS

/* The C compiler we're using */
#undef HAVE_CC

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
#undef HAVE_DCGETTEXT

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#undef HAVE_DIRENT_H

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define to 1 if you have the `getopt' function. */
#undef HAVE_GETOPT

/* Define to 1 if you have the <getopt.h> header file. */
#undef HAVE_GETOPT_H

/* Define to 1 if you have the `getopt_long' function. */
#undef HAVE_GETOPT_LONG

/* Define if the GNU gettext() function is already present or preinstalled. */
#undef HAVE_GETTEXT

/* Define to 1 if you have the `gmtime_r' function. */
#undef HAVE_GMTIME_R

/* Define if you have the iconv() function. */
#undef HAVE_ICONV

/* Define to 1 if you have the `inet_aton' function. */
#undef HAVE_INET_ATON

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the <langinfo.h> header file. */
#undef HAVE_LANGINFO_H

/* whether we compile with libexif support */
#undef HAVE_LIBEXIF

/* whether we use a version of libexif with ExifData.ifd[[]] */
#undef HAVE_LIBEXIF_IFD

/* whether we compile with gdlib support */
#undef HAVE_LIBGD

/* Define to 1 if you have the `ibs' library (-libs). */
#undef HAVE_LIBIBS

/* define if we found LIBJPEG and its headers */
#undef HAVE_LIBJPEG

/* whether we compile with libxml-2.0 support */
#undef HAVE_LIBXML2

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <locale.h> header file. */
#undef HAVE_LOCALE_H

/* Define to 1 if you have the `localtime_r' function. */
#undef HAVE_LOCALTIME_R

/* Define to 1 if you have the `lstat' function. */
#undef HAVE_LSTAT

/* Whether libltdl (of libtool fame) is present */
#undef HAVE_LTDL

/* Define to 1 if you have the <mcheck.h> header file. */
#undef HAVE_MCHECK_H

/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Define to 1 if you have the `mkdir' function. */
#undef HAVE_MKDIR

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
#undef HAVE_NDIR_H

/* Define to 1 if you have the `rand_r' function. */
#undef HAVE_RAND_R

/* Define to 1 if you have the `snprintf' function. */
#undef HAVE_SNPRINTF

/* Define to 1 if you have the `sprintf' function. */
#undef HAVE_SPRINTF

/* Define to 1 if you have the `statfs' function. */
#undef HAVE_STATFS

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#undef HAVE_STDLIB_H

/* Define to 1 if you have the `strcpy' function. */
#undef HAVE_STRCPY

/* Define to 1 if you have the `strdup' function. */
#undef HAVE_STRDUP

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#undef HAVE_STRING_H

/* Define to 1 if you have the `strncpy' function. */
#undef HAVE_STRNCPY

/* Define to 1 if `f_bavail' is a member of `struct statfs'. */
#undef HAVE_STRUCT_STATFS_F_BAVAIL

/* Define to 1 if `f_bfree' is a member of `struct statfs'. */
#undef HAVE_STRUCT_STATFS_F_BFREE

/* Define to 1 if `f_favail' is a member of `struct statfs'. */
#undef HAVE_STRUCT_STATFS_F_FAVAIL

/* Define to 1 if `f_blocks' is a member of `struct statvfs'. */
#undef HAVE_STRUCT_STATVFS_F_BLOCKS

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
#undef HAVE_SYS_DIR_H

/* Define to 1 if you have the <sys/mman.h> header file. */
#undef HAVE_SYS_MMAN_H

/* Define to 1 if you have the <sys/mount.h> header file. */
#undef HAVE_SYS_MOUNT_H

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
#undef HAVE_SYS_NDIR_H

/* Define to 1 if you have the <sys/param.h> header file. */
#undef HAVE_SYS_PARAM_H

/* Define to 1 if you have the <sys/select.h> header file. */
#undef HAVE_SYS_SELECT_H

/* Define to 1 if you have the <sys/statfs.h> header file. */
#undef HAVE_SYS_STATFS_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/time.h> header file. */
#undef HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. */
#undef HAVE_SYS_TYPES_H

/* Define to 1 if you have the <sys/user.h> header file. */
#undef HAVE_SYS_USER_H

/* Define to 1 if you have the <sys/vfs.h> header file. */
#undef HAVE_SYS_VFS_H

/* whether struct tm has tm_gmtoff field */
#undef HAVE_TM_GMTOFF

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Whether we have the va_copy() function */
#undef HAVE_VA_COPY

/* Define to 1 if you have the `vsnprintf' function. */
#undef HAVE_VSNPRINTF

/* Define as const if the declaration of iconv() needs const. */
#undef ICONV_CONST

/* Whether the set of camlibs built is incomplete */
#undef INCOMPLETE_CAMLIB_SET

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#undef LT_OBJDIR

/* gphoto development mailing list */
#define MAIL_GPHOTO_DEVEL "<gphoto-devel@...>"

/* gphoto translation mailing list */
#undef MAIL_GPHOTO_TRANSLATION

/* gphoto user mailing list */
#define MAIL_GPHOTO_USER "<gphoto-user@...>"

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
#undef NO_MINUS_C_MINUS_O

/* Name of package */
#define PACKAGE "libgphoto2-2.5.24"

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#define PACKAGE_NAME "libgphoto2"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libgphoto2-2.5.24"

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the home page for this package. */
#undef PACKAGE_URL

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.5.24"

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
#undef TM_IN_SYS_TIME

/* camera list with support status */
#undef URL_DIGICAM_LIST

/* gphoto project home page */
#undef URL_GPHOTO_HOME

/* gphoto sourceforge project page */
#undef URL_GPHOTO_PROJECT

/* jphoto home page */
#undef URL_JPHOTO_HOME

/* information about using USB mass storage */
#define URL_USB_MASSSTORAGE "http://www.linux-usb.org/USB-guide/x498.html"

/* Version number of package */
#define VERSION "2.5.24"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
#  undef WORDS_BIGENDIAN
# endif
#endif

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
#undef _FILE_OFFSET_BITS

/* Define for large files, on AIX-style hosts. */
#undef _LARGE_FILES

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#undef inline
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
#undef size_t

/* __va_copy() was the originally proposed name */
#undef va_copy

#include "config-msvc.h"

// empty-define parameter to bindtextdomain() call
#define GETTEXT_PACKAGE ""

// the folder where libgphoto2 searches for camlibs .dlls to load
#define CAMLIBS "camlibs"

// signal that our build has non-standard set of camlibs
#define GP_CAMLIB_SET_IS_NONSTANDARD 1

// list of camlibs for this build
#define GP_CAMLIB_SET "ptp2 canon"
