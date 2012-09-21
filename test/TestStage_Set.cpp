/*
 * TestStage_Set.cpp
 *
 *      Author: Angela Italiano
 */
#include <gtest/gtest.h>
#include <models/task.hpp>
#include <models/Stage_Set.hpp>

using namespace std;
using namespace Scan;

class Stage_SetFixture : public ::testing::Test {
protected:

    Stage_Set *tset1;
    Stage_Set *tset2;
	
    Task *t1;
    Task *t2;
    Task *t3;
    Task *t4;
    Task *t5;
    Stage *s1;
    Stage *s2;
    Stage *s3;
    Stage *s4;
    Stage *s5;

    Stage_SetFixture()   {
	tset1=new Stage_Set(); 
	tset2=new Stage_Set(); 
	t1= new Task();
	t2= new Task();
	t3= new Task("A");
	t4= new Task("B");
	t5= new Task();	


    t1->set_period(20); t1->set_wcet(1); t1->set_dline(30);
    t2->set_period(20); t2->set_wcet(3); t2->set_dline(30);
    t3->set_period(20); t3->set_wcet(5); t3->set_dline(30);
    t4->set_period(20); t4->set_wcet(7); t4->set_dline(30);
    t5->set_period(30); t5->set_wcet(18); t5->set_dline(40);
	s1=new Stage(*t1,0,0);
	s2=new Stage(*t2,0,3);
	s3=new Stage(*t3,0,4);
	s4=new Stage(*t4,0,8);
	s5=new Stage(*t5,1,0);
	*tset1+=*s1;
	*tset1+=*s2;
	*tset1+=*s3;
	*tset1+=*s4;
	*tset2+=*s5;   

    }
};

TEST_F(Stage_SetFixture, TestGetAt)
{
    EXPECT_EQ(16, tset1->get_tot_wcets());
    EXPECT_EQ(20, tset1->get_periods().at(1));
    EXPECT_EQ(30, tset1->get_dlines().at(1));
    EXPECT_EQ(7, tset1->get_wcets().at(3));
    EXPECT_EQ(4, tset1->size());
    Stage ps1 = (*tset1)[0];
    Stage ps2 = tset1->at(0);
    EXPECT_EQ(ps1,ps2);
    EXPECT_EQ(0.8,tset1->get_util());
}



TEST_F(Stage_SetFixture, TestBwParam)
{
	 Stage_Set *tset3=new Stage_Set();
	*tset3+=*s2;
	*tset3+=*s3; 
    Stage_Set *tset4=new Stage_Set(); 
	*tset4+=*s1;
	*tset4+=*s3;
	*tset4+=*s4;
 EXPECT_EQ(4, tset1->size()); 
	// due task consecutivi all'interno
	 EXPECT_EQ(27, tset1->get_bw_param());
	//unico task
	 EXPECT_EQ(18, tset2->get_bw_param());
	//task consecutivi
 	EXPECT_EQ(8, tset3->get_bw_param());
	// due task non consecutivi	
	EXPECT_EQ(21, tset4->get_bw_param());
}
TEST_F(Stage_SetFixture, TestSum)
{   
    *tset1 += *tset2;
    EXPECT_EQ(5, tset1->size());
    EXPECT_EQ(18,tset1->get_wcets().at(4));  
}



