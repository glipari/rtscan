top=../

AM_CPPFLAGS=-I${top}
AM_CXXFLAGS=-Wall -std=c++0x
AM_LDFLAGS=-L${top}common -L${top}models -L${top}analysis 

SUBDIRS = mbwi_exps/ hyperplane_example/ ppl/

noinst_PROGRAMS = simple

simple_SOURCES = simple.cpp
simple_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem