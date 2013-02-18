top=../

SUBDIRS=
EXTRA_DIST=

AM_CPPFLAGS=-I${top}
AM_CXXFLAGS=-Wall -std=c++0x

lib_LTLIBRARIES=libscan_models.la
libscan_models_la_SOURCES=task.cpp taskset.cpp \
	transaction.cpp has_critical_section.cpp \
	taskres_parser.cpp task_parser.cpp \
    fp_task.cpp \
	computational_resource.cpp \
	pipeline_property_parser.cpp \
	pipeline.cpp
