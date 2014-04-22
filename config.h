/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Anjuta major version */
#define ANJUTA_MAJOR_VERSION 3

/* Anjuta micro version */
#define ANJUTA_MICRO_VERSION 1

/* Anjuta minor version */
#define ANJUTA_MINOR_VERSION 11

/* Anjuta version */
#define ANJUTA_VERSION 3.11.1

/* Need to implement our own forkpty() */
/* #undef EMULATE_FORKPTY */

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#define ENABLE_NLS 1

/* define to enable PackageKit installer */
#define ENABLE_PACKAGEKIT 1

/* GETTEXT package name */
#define GETTEXT_PACKAGE "anjuta"

/* Define to 1 if you have the MacOS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
/* #undef HAVE_CFLOCALECOPYCURRENT */

/* Define to 1 if you have the MacOS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
/* #undef HAVE_CFPREFERENCESCOPYAPPVALUE */

/* Define to 1 if you have the `chmod' function. */
/* #undef HAVE_CHMOD */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
#define HAVE_DCGETTEXT 1

/* Define to 1 if you have the declaration of `lockf', and to 0 if you don't.
   */
#define HAVE_DECL_LOCKF 1

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fgetpos' function. */
#define HAVE_FGETPOS 1

/* Define to 1 if you have the `fnmatch' function. */
#define HAVE_FNMATCH 1

/* Define to 1 if you have the <fnmatch.h> header file. */
#define HAVE_FNMATCH_H 1

/* Define to 1 if you have the `getline' function. */
#define HAVE_GETLINE 1

/* Define if the GNU gettext() function is already present or preinstalled. */
#define HAVE_GETTEXT 1

/* Define if you have the iconv() function and it works. */
/* #undef HAVE_ICONV */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the <libutil.h> header file. */
/* #undef HAVE_LIBUTIL_H */

/* Define to 1 if you have the `lockf' function. */
#define HAVE_LOCKF 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if you have the <pty.h> header file. */
#define HAVE_PTY_H 1

/* Define if libXrender is available. */
#define HAVE_RENDER 1

/* Define to 1 if you have the <stat.h> header file. */
/* #undef HAVE_STAT_H */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `stricmp' function. */
/* #undef HAVE_STRICMP */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strnicmp' function. */
/* #undef HAVE_STRNICMP */

/* Define to 1 if you have the `strstr' function. */
#define HAVE_STRSTR 1

/* Define to 1 if you have the <sys/dir.h> header file. */
#define HAVE_SYS_DIR_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/times.h> header file. */
#define HAVE_SYS_TIMES_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the <types.h> header file. */
/* #undef HAVE_TYPES_H */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <util.h> header file. */
/* #undef HAVE_UTIL_H */

/* Defined if devhelp is built against webkitgtk2 */
/* #undef HAVE_WEBKIT2 */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Do not know */
/* #undef NEED_PROTO_FGETPOS */

/* Do not know */
/* #undef NEED_PROTO_FTRUNCATE */

/* Do not know */
/* #undef NEED_PROTO_GETENV */

/* Do not know */
/* #undef NEED_PROTO_LSTAT */

/* Do not know */
/* #undef NEED_PROTO_MALLOC */

/* Do not know */
/* #undef NEED_PROTO_REMOVE */

/* Do not know */
/* #undef NEED_PROTO_STAT */

/* Do not know */
/* #undef NEED_PROTO_TRUNCATE */

/* Do not know */
/* #undef NEED_PROTO_UNLINK */

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "anjuta"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=anjuta"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Anjuta"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Anjuta 3.11.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "anjuta"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://www.anjuta.org/"

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.11.1"

/* Suffix to add to preferences directory */
#define PREF_SUFFIX ""

/* Seek set 0 */
/* #undef SEEK_SET */

/* Define to 1 if you have the ANSI C header files. */
/* #undef STDC_HEADERS */

/* Directory for temporary files */
#define TMPDIR "/tmp"

/* Version number of package */
#define VERSION "3.11.1"

/* Define to 1 to internationalize bison runtime messages. */
#define YYENABLE_NLS 1

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1
