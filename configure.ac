#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([statfs-faker], [0.99], [https://github.com/fumiyas/linux-statfs-faker])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([statfs-faker], [chmod +x statfs-faker])

# Checks for programs.
AC_PROG_CC

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

#
# Platform specific setup
#
#############################
AC_CANONICAL_HOST
# Check for which host we are on and setup a few things
# specifically based on the host
case $host_os in
darwin*)
  AC_SUBST(LD_PRELOAD, DYLD_INSERT_LIBRARIES)
  AC_SUBST(DYLD_FORCE_FLAT_NAMESPACE, DYLD_FORCE_FLAT_NAMESPACE)
  AC_SUBST(SHARED_LIB_EXT, dylib)
  ;;
*)
  AC_SUBST(LD_PRELOAD, LD_PRELOAD)
  AC_SUBST(DYLD_FORCE_FLAT_NAMESPACE, '')
  AC_SUBST(SHARED_LIB_EXT, so)
  ;;
esac

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
