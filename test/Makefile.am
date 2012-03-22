top=../
gtest_dir=${top}../gmock-1.6.0/gtest

SUBDIRS=

AM_CXXFLAGS=-Wall -std=c++0x -I${top} -I${gtest_dir}/include/
TESTS = scantest

check_PROGRAMS = scantest
scantest_LDFLAGS=-L${gtest_dir}/lib/ -L${top}common -L${top}models -L${top}analysis   
scantest_LDADD= -lgtest -lpthread -lscan_analysis -lscan_models -lscan_common -lboost_filesystem 
scantest_SOURCES = DBFTest.cpp ParseTest.cpp  TaskSetTest.cpp  TaskTest.cpp test_main.cpp TestUtility.cpp TestFP.cpp

#TestBasicFunctions.cpp  
