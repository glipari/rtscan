#include "gtest/gtest.h"

#include <vector>
#include <algorithm>
#include <models/taskres.hpp>
#include <analysis/fmlp.hpp>

using namespace Scan;
using namespace std;

class FMLPAnalysisTest : public FMLPAnalysis {
public:
    template<class TaskIter, class ResIter>
    FMLPAnalysisTest(int nproc, TaskIter tset_begin, TaskIter tset_end, ResIter res_begin, ResIter res_end) :
        FMLPAnalysis(nproc, tset_begin, tset_end, res_begin, res_end) {}
    using FMLPAnalysis::compute_groups;
    using FMLPAnalysis::compute_np;
    using FMLPAnalysis::compute_spin;
    using FMLPAnalysis::compute_np_blocking;
    using FMLPAnalysis::compute_direct_blocking;
};


TEST(FMLPTest, Groups1)
{
    vector<Resource> vres = {
        Resource(true),
        Resource(false),
    };

    vector<TaskRes> tset = {
        TaskRes(10,50,50),
        TaskRes(20,100,100),
        TaskRes(30,150,150),
        TaskRes(40,200,200),
    };
    tset[0].addCS(CriticalSection(vres[0].get_id(), 2));
    tset[1].addCS(CriticalSection(vres[0].get_id(), 4));

    tset[2].addCS(CriticalSection(vres[1].get_id(), 3));
    tset[3].addCS(CriticalSection(vres[1].get_id(), 5));

    FMLPAnalysisTest f(2, tset.begin(), tset.end(), vres.begin(), vres.end());
    
    ResGroupSet gs = f.compute_groups();

    EXPECT_EQ(2, gs.size());
    EXPECT_EQ(1, gs[0].size());
    EXPECT_EQ(1, gs[1].size());
    EXPECT_EQ(vres[0].get_id(), (gs[0].begin())->get_id());
    EXPECT_EQ(vres[1].get_id(), (gs[1].begin())->get_id());
}

TEST(FMLPTest, Groups2)
{
    vector<Resource> vres = {
        Resource(true),
        Resource(false),
        Resource(true),
        Resource(false)
    };

    vector<TaskRes> tset = {
        TaskRes(10,50,50),
        TaskRes(20,100,100),
        TaskRes(30,150,150),
        TaskRes(40,200,200),
    };

    CriticalSection cs0_1(vres[2].get_id(), 2); 
    cs0_1.addNestedCS(CriticalSection(vres[0].get_id(), 1));
    tset[0].addCS(cs0_1);
    CriticalSection cs0_2(vres[1].get_id(), 8); 
    cs0_2.addNestedCS(CriticalSection(vres[0].get_id(), 1));
    tset[0].addCS(cs0_2);

    CriticalSection cs1_1(vres[1].get_id(), 10);
    cs1_1.addNestedCS(CriticalSection(vres[3].get_id(), 9));
    
    tset[1].addCS(cs1_1);

    tset[2].addCS(CriticalSection(vres[1].get_id(), 3));
    tset[3].addCS(CriticalSection(vres[2].get_id(), 5));

    FMLPAnalysisTest f(2, tset.begin(), tset.end(), vres.begin(), vres.end());
    
    ResGroupSet gs = f.compute_groups();

    EXPECT_EQ(2, gs.size());
    EXPECT_EQ(2, gs[0].size());
    EXPECT_EQ(2, gs[1].size());
    int k = 0;
    for (ResGroup::iterator i = gs[0].begin(); i!= gs[0].end(); ++i, ++k) {
        EXPECT_EQ(vres[2*k].get_id(), i->get_id());
    }

    k = 0;
    for (ResGroup::iterator i = gs[1].begin(); i!= gs[1].end(); ++i, ++k) {
        EXPECT_EQ(vres[2*k+1].get_id(), i->get_id());
    }
}

TEST(FMLPTest, Groups3)
{
    vector<Resource> vres = {
        Resource(true),
        Resource(true),
        Resource(true),
        Resource(false)
    };

    vector<TaskRes> tset = {
        TaskRes(10,50,50),
        TaskRes(20,100,100),
        TaskRes(30,150,150),
        TaskRes(40,200,200),
    };
    
    CriticalSection cs0_1(vres[0].get_id(), 2); 
    cs0_1.addNestedCS(CriticalSection(vres[2].get_id(), 1));
    tset[0].addCS(cs0_1);
    CriticalSection cs0_2(vres[1].get_id(), 8); 
    cs0_2.addNestedCS(CriticalSection(vres[2].get_id(), 1));
    tset[0].addCS(cs0_2);

    CriticalSection cs1_1(vres[3].get_id(), 10);
    cs1_1.addNestedCS(CriticalSection(vres[2].get_id(), 9));
    
    tset[1].addCS(cs1_1);

    FMLPAnalysisTest f(2, tset.begin(), tset.end(), vres.begin(), vres.end());
    
    ResGroupSet gs = f.compute_groups();

    EXPECT_EQ(2, gs.size());
    EXPECT_EQ(3, gs[0].size());
    EXPECT_EQ(1, gs[1].size());
    int k = 0;
    for (ResGroup::iterator i = gs[0].begin(); i!= gs[0].end(); ++i, ++k) {
        EXPECT_EQ(vres[k].get_id(), i->get_id());
    }
    
    for (ResGroup::iterator i = gs[1].begin(); i!= gs[1].end(); ++i, ++k) {
        EXPECT_EQ(vres[k].get_id(), i->get_id());
    }
}


TEST(FMLPTest, ShortOnly)
{
    vector<Resource> vres = {
        Resource(true),
        Resource(true),
        Resource(true)
    };

    vector<TaskRes> tset = {
        TaskRes(10,50,50),
        TaskRes(20,100,100),
        TaskRes(30,150,150),
        TaskRes(40,200,200),
    };

    CriticalSection cs0(vres[0].get_id(), 3);
    tset[0].addCS(cs0);
    
    CriticalSection cs1(vres[0].get_id(), 2);
    cs1.addNestedCS(CriticalSection(vres[1].get_id(), 1));
    tset[1].addCS(cs1);

    CriticalSection cs2(vres[1].get_id(), 4);
    tset[2].addCS(cs2);

    tset[3].addCS(CriticalSection(vres[2].get_id(), 2));
    tset[3].addCS(CriticalSection(vres[1].get_id(), 1));

    FMLPAnalysisTest f(2, tset.begin(), tset.end(), vres.begin(), vres.end());
    f.compute_groups();
    f.compute_np();

    // only two processors, so only 4
    EXPECT_EQ(4, f.compute_spin(tset[0], vres[0]));
    EXPECT_EQ(4, f.compute_spin(tset[0], vres[1]));

    EXPECT_EQ(4, f.compute_spin(tset[1], vres[0]));
    EXPECT_EQ(4, f.compute_spin(tset[1], vres[1]));

    EXPECT_EQ(3, f.compute_spin(tset[2], vres[0]));
    EXPECT_EQ(3, f.compute_spin(tset[2], vres[1]));

    EXPECT_EQ(4, f.compute_spin(tset[3], vres[0]));
    EXPECT_EQ(4, f.compute_spin(tset[3], vres[1]));
    EXPECT_EQ(0, f.compute_spin(tset[3], vres[2]));

    // npb[0] from task tset[2], which spins for 3, then uses 4
    EXPECT_EQ(4+3, f.compute_np_blocking(tset[0]));

    // npb[1] from task tset[2]
    EXPECT_EQ(4+3, f.compute_np_blocking(tset[1]));

    // npb[2] from task tset[3]
    EXPECT_EQ(4+1, f.compute_np_blocking(tset[2]));

    // npb[2] from task tset[0]
    EXPECT_EQ(0, f.compute_np_blocking(tset[3]));

    for (int i=0; i<tset.size(); ++i) 
        EXPECT_EQ(0, f.compute_direct_blocking(tset[i]));

    FMLPAnalysisTest f2(4, tset.begin(), tset.end(), vres.begin(), vres.end());
    f2.compute_groups();
    f2.compute_np();

    // four processors, so 7
    EXPECT_EQ(7, f2.compute_spin(tset[0], vres[0]));
    EXPECT_EQ(7, f2.compute_spin(tset[0], vres[1]));
    
    EXPECT_EQ(8, f2.compute_spin(tset[1], vres[0]));
    EXPECT_EQ(8, f2.compute_spin(tset[1], vres[1]));

    EXPECT_EQ(6, f2.compute_spin(tset[2], vres[0]));
    EXPECT_EQ(6, f2.compute_spin(tset[2], vres[1]));

    EXPECT_EQ(9, f2.compute_spin(tset[3], vres[0]));
    EXPECT_EQ(9, f2.compute_spin(tset[3], vres[1]));
    EXPECT_EQ(0, f2.compute_spin(tset[3], vres[2]));

    // npb[0] from task tset[2], which spins for 3, then uses 4
    EXPECT_EQ(4+6, f2.compute_np_blocking(tset[0]));

    // npb[1] from task tset[2]
    EXPECT_EQ(4+6, f2.compute_np_blocking(tset[1]));

    // npb[2] from task tset[3]
    EXPECT_EQ(9+1, f2.compute_np_blocking(tset[2]));

    // npb[2] from task tset[0]
    EXPECT_EQ(0, f2.compute_np_blocking(tset[3]));

    for (int i=0; i<tset.size(); ++i) 
        EXPECT_EQ(0, f2.compute_direct_blocking(tset[i]));
}

