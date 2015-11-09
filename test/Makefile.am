top=../

SUBDIRS=

AM_CXXFLAGS=-Wall -std=gnu++0x -I${top}
TESTS = scantest

check_PROGRAMS = scantest

scantest_LDFLAGS= -L${top}common -L${top}models -L${top}analysis -L/usr/local/lib/ 

scantest_LDADD= -lpthread -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lppl

scantest_SOURCES = test_main.cpp \
	DBFTest.cpp TaskSetTest.cpp  \
	TaskTest.cpp \
	TestFP.cpp \
	CriticalSectionsTest.cpp \
	TaskSet2Test.cpp \
	MBWITest.cpp \
	MEDFTest.cpp \
	HasProcessorTest.cpp \
	FMLPTest.cpp \
	OMLPTest.cpp \
	TaskResParserTest.cpp \
	TestCommon.cpp \
	TestHyperplanes.cpp \
	TestPropertyParser.cpp \
	TestFP_ptr.cpp \
	TestComputationalResource.cpp \
	TestPipeline.cpp \
	TestUtility.cpp \
	TestSchedGFP.cpp

