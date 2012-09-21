/*
 * Test_CheckFeasibility.cpp
 *
 *      Author: Angela Italiano
*/
#include <gtest/gtest.h>
#include <models/CheckFeasibility.hpp>
#include <map>
using namespace std;
using namespace Scan;

class Test_CheckFeasFixture : public ::testing::Test {
protected:
CheckFeasibility * ch;
vector<TaskSet> v_s;
vector<Processor> v_p;
vector< pair<double,double> > p_d;
TaskSet *tset1;
TaskSet *tset2;
TaskSet *tset3;
Task *t1;
Task *t2;
Task *t3;
Task *t4;
Task *t5;
Task *t6;
Processor*p;
Processor*p1;



Test_CheckFeasFixture ()
{
	
	tset1=new TaskSet(); 
	tset2=new TaskSet(); 
	t1= new Task();
	t2= new Task();
	t3= new Task("A");
	t4= new Task("B");
	t5= new Task();
	t6= new Task();		
    t1->set_period(20); t1->set_wcet(1); t1->set_dline(30);
    t2->set_period(20); t2->set_wcet(3); t2->set_dline(30);
    t3->set_period(20); t3->set_wcet(5); t3->set_dline(30);
    t4->set_period(20); t4->set_wcet(7); t4->set_dline(30);
    t5->set_period(30); t5->set_wcet(18);t5->set_dline(40);
	t6->set_period(30); t5->set_wcet(3);t5->set_dline(40);
	*tset1+=*t1;
	*tset1+=*t2;
	*tset1+=*t3;
	*tset1+=*t4;
	*tset2+=*t5;
	*tset2+=*t6; 
	/* ho costruito 2 pipe:
	* la prima t1->t2->t3->t4
	* la seconda t5->t6 */
	p= new Processor(0.4);
	p1= new Processor(0.2);
	v_p.push_back(*p);
	v_p.push_back(*p1);
	v_s.push_back(*tset1);
	v_s.push_back(*tset2);
	/* period e deadline delle due pipe */
	pair<double,double> temp(20,30);
	p_d.push_back(temp);
	pair<double,double> temp1(30,40);
	p_d.push_back(temp1);
	ch= new CheckFeasibility(1,1,v_s,p_d,v_p,0);
}

};


TEST_F(Test_CheckFeasFixture , Test_fun)
{
	/* le combinazioni per una pipe di 
	* 4 task distribuibili in un sistema
	* con due processori */	
	ch->split(2,4);
	/* prendo la combinazione 0 per una pipe lunga 2*/
	vector<int> temp= ch->get_combination(2,0);
	/* la prima combinazione è 11 ovvero entrambi
	* i task vanno sullo stesso processore */
	EXPECT_EQ(2, temp.size());
	EXPECT_EQ(1, temp.at(0));
	EXPECT_EQ(1, temp.at(1));
	/* non è l'ultima combinazione generata */
	EXPECT_EQ(false, ch->is_last_combination(0, 2, 4));
	/* la seconda combinazione è 12 ovvero 
	* i task vanno processori diversi */
	temp=ch->get_combination(2,1);
	EXPECT_EQ(2, temp.at(0));
	EXPECT_EQ(1, temp.at(1));
	/* è l' ultima combinazione */
	EXPECT_EQ(true, ch->is_last_combination(1, 2, 4));


	temp=ch->get_combination(4,2); //1211
	vector<Stage_Set>vec_s=ch->get_Stage_Set(temp, 0);
	/* i task vanno {t1,t3,t4} in un task
	* set , {t2} va in un altro task set */
	EXPECT_EQ(3, vec_s.at(0).size());
	EXPECT_EQ(1, vec_s.at(1).size());
	EXPECT_EQ(13,vec_s.at(0).get_tot_wcets());
	EXPECT_EQ(3,vec_s.at(1).get_tot_wcets());

	/* primo task set
	 * 1+ 1+ (12)=14 dove 12= 5+7 wcets task consecutivi
	 * 14 /[(1/20 +1/5 +/1/7)=0,65]	=21,5385
	 * secondo task set
	 * 3
	 * 3 / (3/20)=20
	*/ 
	//EXPECT_EQ(41.5385, ch->compute_partialBW(vec_s));



	/* ch settato a worstFit per i processori
	 * restituisce l'indie 1 che corrisponde a p1 
	 * (che ha bw gia usata pari a 0.2)
	 * nel vettore dei processori */
	EXPECT_EQ(0, ch->select_Processor(&v_p,0.1));

ch= new CheckFeasibility(1,0,v_s,p_d,v_p,0);

/* ch settato a bestFit per i processori
	 * restituisce l'indie 1 che corrisponde a p 
	 * (che ha bw gia usata pari a 0.2)
	 * nel vettore dei processori */
	EXPECT_EQ(1, ch->select_Processor(&v_p,0.1));
	

	
	
	
	
}
