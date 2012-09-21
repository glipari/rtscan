top=../

AM_CPPFLAGS=-I${top}
AM_CXXFLAGS=-Wall -std=c++0x
AM_LDFLAGS=-L${top}common -L${top}models 


noinst_PROGRAMS = main

main_SOURCES = main.cpp
main_LDADD =  -lscan_models -lscan_common -lboost_filesystem 

