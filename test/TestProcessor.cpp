
/*
 * TestProcessor.cpp
 *
 *      Author: Angela Italiano
 */
#include <gtest/gtest.h>
#include <models/Processor.hpp>

using namespace std;
using namespace Scan;
class ProcessorFixture : public ::testing::Test {
protected:
  	Processor * p;
	Processor * p1;
	
ProcessorFixture()   {
	p= new Processor(0.6);
 	p1= new Processor(*p);
}    
};

TEST_F(ProcessorFixture, TestParam)
{
    EXPECT_EQ(0.6, p->get_utilised_Bw());
    EXPECT_EQ(0.4, p->get_free());
	EXPECT_EQ(0.6,  p1->get_utilised_Bw());
    EXPECT_EQ(false, p->get_flag_utilised());
	p->set_flag_utilised(true);
    EXPECT_EQ(true, p->get_flag_utilised());
    
}

