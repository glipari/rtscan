/*
 * Proc_Allocation.cpp
 *
 *      Author: Angela Italiano
 */
#include <gtest/gtest.h>
#include <models/Proc_Allocation.hpp>

using namespace std;
using namespace Scan;

class Proc_AllocationFixture : public ::testing::Test {
protected:
Processor *p;
Processor *p1;
Procs_Allocation *pa1;
Procs_Allocation *pa2;
vector<Procs_Allocation> v;
Feasible_Allcoation *fa;
Stage_Set *tset1;
Stage_Set *tset2;
Stage_Set *tset3;
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

Proc_AllocationFixture ()
{
	tset1=new Stage_Set(); 
	tset2=new Stage_Set(); 
	tset3=new Stage_Set(); 
	t1= new Task();
	t2= new Task();
	t3= new Task("A");
	t4= new Task("B");
	t5= new Task();		
    t1->set_period(20); t1->set_wcet(1); t1->set_dline(30);
    t2->set_period(20); t2->set_wcet(3); t2->set_dline(30);
    t3->set_period(20); t3->set_wcet(5); t3->set_dline(30);
    t4->set_period(20); t4->set_wcet(7); t4->set_dline(30);
    t5->set_period(30); t5->set_wcet(18);t5->set_dline(40);
	s1=new Stage(*t1,0,0);
	s2=new Stage(*t2,0,1);
	s3=new Stage(*t3,0,2);
	s4=new Stage(*t4,0,3);
	s5=new Stage(*t5,1,0);
	*tset1+=*s1;
	*tset2+=*s2;
	*tset1+=*s3;
	*tset2+=*s4;
	*tset3+=*s5; 
	p= new Processor(0.4);
	p1= new Processor(0.2);
	pa1=new Procs_Allocation(*p,0,*tset1);
	pa2=new Procs_Allocation(*p1,0,*tset2);
	pa1->update_task_allocated(*p,1,*tset3);
	v.push_back(*pa1);
	v.push_back(*pa2);
	fa= new Feasible_Allcoation(v);
	
	
}

};

TEST_F(Proc_AllocationFixture, TestPro_All)
{
	Processor p2=pa1->get_Processor();
   	EXPECT_EQ(p->get_Id(), p2.get_Id());
	EXPECT_EQ(6,pa1->get_at(0).get_tot_wcets());
	EXPECT_EQ(18,pa1->get_at(1).get_tot_wcets());
}
TEST_F(Proc_AllocationFixture, TestFeasible)
{

   	EXPECT_EQ(2, fa->size());
	EXPECT_EQ(6,fa->get_at(0).get_at(0).get_tot_wcets());
	EXPECT_EQ(18,fa->get_at(0).get_at(1).get_tot_wcets());
	
}

