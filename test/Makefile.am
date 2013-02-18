top=../
gtest_dir=${top}../gtest/gtest-1.6.0/

SUBDIRS=

#AM_CXXFLAGS=-Wall -std=c++0x -I${top} -I${gtest_dir}/include/
AM_CXXFLAGS=-Wall -std=gnu++0x -I${top} -I${gtest_dir}/include/
TESTS = scantest

check_PROGRAMS = scantest
scantest_LDFLAGS=-L${gtest_dir}/lib/ -L${top}common -L${top}models -L${top}analysis -L/usr/local/lib/ 
scantest_LDADD= -lgtest -lpthread -lscan_analysis -lscan_models -lscan_common -lboost_filesystem -lppl
scantest_SOURCES = test_main.cpp \
	DBFTest.cpp TaskSetTest.cpp  \
	TaskTest.cpp TestFP.cpp \
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
    TestUtility.cpp 

#TestBasicFunctions.cpp  
