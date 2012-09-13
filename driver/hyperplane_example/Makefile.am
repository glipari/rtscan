top=../../

AM_CPPFLAGS=-I${top}
AM_CXXFLAGS=-Wall -std=c++0x -fopenmp
AM_LDFLAGS=-L${top}common -L${top}models -L${top}analysis 

noinst_PROGRAMS = example1

example1_SOURCES = example1.cpp

example1_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lgomp