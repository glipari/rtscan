top=../../

AM_CPPFLAGS=-I${top}
AM_CXXFLAGS=-Wall -std=c++0x -fopenmp
AM_LDFLAGS=-L${top}common -L${top}models -L${top}analysis 

noinst_PROGRAMS = edf_comp

edf_comp_SOURCES = edf_comp.cpp

edf_comp_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lgomp