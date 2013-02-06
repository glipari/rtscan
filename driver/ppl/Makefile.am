top=../../

AM_CPPFLAGS=-I${top} -I/usr/include/ppl
AM_CXXFLAGS=-Wall
AM_LDFLAGS=-L${top}common -L${top}models -L${top}analysis -L/usr/local/lib/

noinst_PROGRAMS = sensitivity distributed edf_ex edf_sim np_sensitivity syc_sensitivity

sensitivity_SOURCES = sensitivity.cpp #ppl_utils.cpp
sensitivity_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl

syc_sensitivity_SOURCES = syc_sensitivity.cpp #ppl_utils.cpp
syc_sensitivity_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl

np_sensitivity_SOURCES = np_sensitivity.cpp #ppl_utils.cpp
np_sensitivity_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl

distributed_SOURCES = distributed.cpp #ppl_utils.cpp
distributed_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl

edf_ex_SOURCES = edf_ex.cpp #ppl_utils.cpp
edf_ex_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl

edf_sim_SOURCES = edf_sim.cpp #ppl_utils.cpp
edf_sim_LDADD = -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lboost_program_options -lppl
