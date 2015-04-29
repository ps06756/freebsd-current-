/* $FreeBSD: head/lib/clang/include/clang/Config/config.h 280031 2015-03-15 13:31:13Z dim $ */
/* tools/clang/include/clang/Config/config.h.  Generated from config.h.in by configure.  */
/* This generated file is for internal use. Do not include it from headers. */

#ifdef CONFIG_H
#error config.h can only be included once
#else
#define CONFIG_H

/* Bug report URL. */
#define BUG_REPORT_URL "https://bugs.freebsd.org/submit/"

/* Multilib suffix for libdir. */
#define CLANG_LIBDIR_SUFFIX ""

/* Relative directory for resource files */
#define CLANG_RESOURCE_DIR ""

/* Directories clang will search for headers */
#define C_INCLUDE_DIRS ""

/* Default <path> to all compiler invocations for --sysroot=<path>. */
/* #undef DEFAULT_SYSROOT */

/* Directory where gcc is installed. */
#define GCC_INSTALL_PREFIX ""

/* Define if we have libxml2 */
/* #undef CLANG_HAVE_LIBXML */

#define PACKAGE_STRING "LLVM 3.6.0"

/* The LLVM product name and version */
#define BACKEND_PACKAGE_STRING PACKAGE_STRING

/* Linker version detected at compile time. */
/* #undef HOST_LINK_VERSION */

#endif