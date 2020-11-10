/** \file
* \brief configuration for msvc compiler, project libgphoto2_port
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

/* Define if you want to disabled debugging. */
#undef DISABLE_DEBUGGING

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#undef ENABLE_NLS

/* The gettext domain we're using */
#undef GETTEXT_PACKAGE

/* Define to 1 if you have the <asm/io.h> header file. */
#undef HAVE_ASM_IO_H

/* Define if there is asm .symver support. */
#undef HAVE_ASM_SYMVERS

/* Define to 1 if you have the <byteswap.h> header file. */
#undef HAVE_BYTESWAP_H

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

/* Define to 1 if you have the <endian.h> header file. */
#undef HAVE_ENDIAN_H

/* Define to 1 if you have the `endmntent' function. */
#undef HAVE_ENDMNTENT

/* Define to 1 if you have the <errno.h> header file. */
#undef HAVE_ERRNO_H

/* Define to 1 if you have the <fcntl.h> header file. */
#undef HAVE_FCNTL_H

/* Define to 1 if you have the `flock' function. */
#undef HAVE_FLOCK

/* Define if the GNU gettext() function is already present or preinstalled. */
#undef HAVE_GETTEXT

/* Define if you have GNU regex. */
#undef HAVE_GNU_REGEX

/* Define if you have the iconv() function. */
#undef HAVE_ICONV

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the <ioctl-types.h> header file. */
#undef HAVE_IOCTL_TYPES_H

/* Define to 1 if you have the `pthread' library (-lpthread). */
#undef HAVE_LIBPTHREAD

/* Define to 1 if you have the `regex' library (-lregex). */
#undef HAVE_LIBREGEX

/* whether we compile with libusb support */
#define HAVE_LIBUSB

/* whether we compile with libusb-1.0 support */
#define HAVE_LIBUSB1

/* Define if libusb-1.0 has libusb_strerror */
#define HAVE_LIBUSB_STRERROR

/* Define to 1 if you have the <limits.h> header file. */
#undef HAVE_LIMITS_H

/* Define if you have dev_lock/lockdev based locking. */
#undef HAVE_LOCKDEV

/* Whether libltdl (of libtool fame) is present */
#define HAVE_LTDL

/* Define if you have Bonjour/mDNSResponder. */
#undef HAVE_MDNS_BONJOUR

/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Define to 1 if you have the <mntent.h> header file. */
#undef HAVE_MNTENT_H

/* whether struct mnttag has mnt_mountp field */
#undef HAVE_MNTTAB

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
#undef HAVE_NDIR_H

/* whether we have regex.h */
#undef HAVE_REGEX

/* Define if you have TIOCM_TRS. */
#undef HAVE_RTS_IOCTL

/* Define to 1 if you have the <scsi/sg.h> header file. */
#undef HAVE_SCSI_SG_H

/* Whether you have serial support enabled */
#undef HAVE_SERIAL

/* Define to 1 if you have the `setmntent' function. */
#undef HAVE_SETMNTENT

/* Define to 1 if you have the <sgetty.h> header file. */
#undef HAVE_SGETTY_H

/* Define to 1 if you have the <sgtty.h> header file. */
#undef HAVE_SGTTY_H

/* Define to 1 if you have the `snprintf' function. */
#undef HAVE_SNPRINTF

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdio.h> header file. */
#undef HAVE_STDIO_H

/* Define to 1 if you have the <stdlib.h> header file. */
#undef HAVE_STDLIB_H

/* Define to 1 if you have the `strerror' function. */
#undef HAVE_STRERROR

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#undef HAVE_STRING_H

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
#undef HAVE_SYS_DIR_H

/* Define to 1 if you have the <sys/file.h> header file. */
#undef HAVE_SYS_FILE_H

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#undef HAVE_SYS_IOCTL_H

/* Define to 1 if you have the <sys/mntent.h> header file. */
#undef HAVE_SYS_MNTENT_H

/* Define to 1 if you have the <sys/mnttab.h> header file. */
#undef HAVE_SYS_MNTTAB_H

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
#undef HAVE_SYS_NDIR_H

/* Define to 1 if you have the <sys/param.h> header file. */
#undef HAVE_SYS_PARAM_H

/* Define to 1 if you have the <sys/select.h> header file. */
#undef HAVE_SYS_SELECT_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/time.h> header file. */
#undef HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. */
#undef HAVE_SYS_TYPES_H

/* Define to 1 if you have the <termios.h> header file. */
#undef HAVE_TERMIOS_H

/* Define to 1 if you have the <termio.h> header file. */
#undef HAVE_TERMIO_H

/* Define to 1 if you have the <ttold.h> header file. */
#undef HAVE_TTOLD_H

/* Define if you have ttylock based tty locking. */
#undef HAVE_TTYLOCK

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Whether we have the va_copy() function */
#undef HAVE_VA_COPY

/* Define to 1 if you have the `vsnprintf' function. */
#undef HAVE_VSNPRINTF

/* Define as const if the declaration of iconv() needs const. */
#undef ICONV_CONST

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
#define PACKAGE "libgphoto2_port-2.5.26"

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#define PACKAGE_NAME "libgphoto2_port"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libgphoto2_port-2.5.26"

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the home page for this package. */
#undef PACKAGE_URL

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.5.26"

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

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
#define VERSION "2.5.26"

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#undef inline
#endif

/* __va_copy() was the originally proposed name */
#undef va_copy

#include "config-msvc.h"

#define GETTEXT_PACKAGE ""

// define this to omit "dllexport" defines for libpcre
#define PCRE_DEFINITION

// list of iolib dlls in the project
#define IOLIB_LIST "usb1 disk ptpip"

// the folder where libgphoto2 searches for iolibs .dlls to load
#define IOLIBS "iolibs"
