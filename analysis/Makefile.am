top=../

SUBDIRS=
EXTRA_DIST=

AM_LDFLAGS=-L/usr/local/lib/
AM_CPPFLAGS=-Wall -I${top} -std=gnu++0x -lboost_filesystem -lboost_program_options -lppl
lib_LTLIBRARIES=libscan_analysis.la
libscan_analysis_la_SOURCES=dbf.cpp \
	mbwi_interference.cpp \
	fmlp.cpp omlp.cpp constraint_space.cpp hyperplane.cpp \
	ppl_utils.cpp
