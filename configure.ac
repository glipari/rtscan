dnl Process this file with autoconf to produce a configure script

AC_INIT(Scan, 0.1, g.lipari@sssup.it)
AM_INIT_AUTOMAKE([src], 1.0)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CXX
AC_PROG_LIBTOOL

dnl End the configure script.
AC_CHECK_PROGS(RANLIB, ranlib)
AC_OUTPUT(Makefile models/Makefile common/Makefile analysis/Makefile driver/Makefile driver/mbwi_exps/Makefile driver/edf_complexity/Makefile test/Makefile, [date > stamp-h] )
