top=../

AM_CPPFLAGS=-I${top}
AM_CXXFLAGS=-Wall -std=c++0x
AM_LDFLAGS=-L${top}common -L${top}models -L${top}analysis 
noinst_PROGRAMS = simple mbwi_simul
simple_SOURCES = simple.cpp
simple_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem 

mbwi_simul_SOURCES = mbwi_simul.cpp 
mbwi_simul_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem 
