dnl Process this file with autoconf to produce a configure script

AC_INIT(Scan, 0.1, g.lipari@sssup.it)
AM_INIT_AUTOMAKE([src], 1.0)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CXX
AC_LANG(C++)
AC_PROG_LIBTOOL

AC_CHECK_HEADERS([boost/spirit.hpp], [], 
                 [AC_MSG_ERROR(You need the Spirit Boost library)])
AC_CHECK_HEADERS([boost/variant.hpp], [], 
                 [AC_MSG_ERROR(You need the Spirit Variant library)])
AC_CHECK_HEADERS([ppl.hh], [],
                 [AC_MSG_ERROR(You need the PPL "Parma Polyhedra Library")])

AC_CHECK_PROGS(RANLIB, ranlib)
AC_OUTPUT(Makefile models/Makefile common/Makefile analysis/Makefile driver/Makefile driver/mbwi_exps/Makefile driver/edf_complexity/Makefile driver/hyperplane_example/Makefile driver/ppl/Makefile test/Makefile, [date > stamp-h] )

dnl End the configure script.
