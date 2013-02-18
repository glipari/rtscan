top=../../

AM_CPPFLAGS=-I${top} -I/usr/include/ppl 
AM_CXXFLAGS=-Wall -std=gnu++0x
AM_LDFLAGS=-L${top}common -L${top}models -L${top}analysis -L/usr/local/lib/

noinst_PROGRAMS = sensitivity distributed edf_ex edf_sim param_sensitivity

sensitivity_SOURCES = sensitivity.cpp #ppl_utils.cpp
sensitivity_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl

distributed_SOURCES = distributed.cpp #ppl_utils.cpp
distributed_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl

edf_ex_SOURCES = edf_ex.cpp #ppl_utils.cpp
edf_ex_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl

edf_sim_SOURCES = edf_sim.cpp #ppl_utils.cpp
edf_sim_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl

param_sensitivity_SOURCES = param_sensitivity.cpp #ppl_utils.cpp
param_sensitivity_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl 
