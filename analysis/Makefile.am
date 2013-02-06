top=../

SUBDIRS=
EXTRA_DIST=

AM_LDFLAGS=-L/usr/local/lib/
#AM_CPPFLAGS=-Wall -std=c++0x -I${top}
AM_CPPFLAGS=-Wall -I${top} -std=gnu++0x -lboost_filesystem -lboost_program_options -lppl
lib_LTLIBRARIES=libscan_analysis.la
#ppl_utils_SOURCES = ppl_utils.cpp
#ppl_utils_LDADD = -lboost_filesystem -lboost_program_options -lppl
libscan_analysis_la_SOURCES=dbf.cpp \
	mbwi_interference.cpp \
	fmlp.cpp omlp.cpp constraint_space.cpp hyperplane.cpp \
	ppl_utils.cpp
#libscan_analysis_la_LIBADD = -lboost_filesystem -lboost_program_options -lppl
