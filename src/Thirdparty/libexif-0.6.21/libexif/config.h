/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#undef ENABLE_NLS

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
#undef HAVE_DCGETTEXT

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define if the GNU gettext() function is already present or preinstalled. */
#undef HAVE_GETTEXT

/* Define if you have the iconv() function. */
#undef HAVE_ICONV

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the `localtime_r' function. */
#undef HAVE_LOCALTIME_R

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define as const if the declaration of iconv() needs const. */
#undef ICONV_CONST

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#undef LT_OBJDIR

/* Name of package */
#define PACKAGE "libexif-0.6.21"

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#define PACKAGE_NAME "libexif"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libexif-0.6.21"

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the home page for this package. */
#undef PACKAGE_URL

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.6.21"

/* file version as numeric comma-separated values (for versioninfo.rc) */
#define VERSIONINFO_FILEVERSION_NUMERIC 0,6,21,0

/* product version as numeric comma-separated values (for versioninfo.rc) */
#define VERSIONINFO_PRODUCTVERSION_NUMERIC 0,6,21,0

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.6.21"

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#undef inline
#endif

// Win32 only has the one with underscore
#define snprintf _snprintf

// posix extension; signed size_t
typedef int ssize_t;

// empty-define parameter to bindtextdomain() call
#define GETTEXT_PACKAGE ""

// inline is used at some parts; make it unseen
#define inline
