top=../../

AM_CPPFLAGS=-I${top} -I/usr/include/ppl
AM_CXXFLAGS=-Wall
AM_LDFLAGS=-L${top}common -L${top}models -L${top}analysis -L/usr/local/lib/

noinst_PROGRAMS = sensitivity distributed

sensitivity_SOURCES = sensitivity.cpp ppl_utils.cpp
sensitivity_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl

distributed_SOURCES = distributed.cpp ppl_utils.cpp
distributed_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl
