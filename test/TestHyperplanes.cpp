#include "gtest/gtest.h"

#include <models/fp_task.hpp>
#include <analysis/hyperplane.hpp>

using namespace Scan;
using namespace std;

TEST(TestHET, Points1)
{
    vector<FPTask> tset = { {2, 5, 10}, {3, 12, 15} };

    vector<double> p = compute_points(tset.begin(), tset.end(), 12);

    EXPECT_EQ(2, p.size());
    EXPECT_EQ(p[0], 10);
    EXPECT_EQ(p[1], 12);
}

TEST(TestHET, PositiveSpace)
{
    for (int n=2; n<10; n++) {
        Conjunction s = non_negative_space(n);
        for (int i=0; i<n; ++i) {
            Plane p = s.get(i);
            for (int j=0; j<n; ++j) {
                if (j != i) 
                    EXPECT_EQ(0, p.a[j]);
                else 
                    EXPECT_EQ(1, p.a[j]);
            }
            EXPECT_EQ(Plane::gte, p.sign);
            EXPECT_EQ(0, p.b);
        }
    }
    
    Conjunction s1 = non_negative_space(2);
    
    EXPECT_TRUE(s1.contains({1,1}));
    EXPECT_TRUE(s1.contains({1,0}));
    EXPECT_TRUE(s1.contains({0,1}));
    EXPECT_TRUE(s1.contains({0,0}));
    EXPECT_FALSE(s1.contains({-1,1}));
    EXPECT_FALSE(s1.contains({1,-1}));
    EXPECT_FALSE(s1.contains({0,-1}));
}

TEST(TestHET, Plane1)
{
    Plane p1({1,0}, Plane::gte, 1);
    EXPECT_FALSE(p1.contains({0,0}));
    EXPECT_TRUE(p1.contains({2,2}));
    EXPECT_TRUE(p1.contains({1,0}));
    EXPECT_FALSE(p1.contains({0,1}));


    Plane p2({1,1}, Plane::gte, 1);
    EXPECT_FALSE(p2.contains({0,0}));
    EXPECT_TRUE(p2.contains({2,2}));
    EXPECT_TRUE(p2.contains({1,0}));
    EXPECT_TRUE(p2.contains({0,1}));
    EXPECT_TRUE(p2.contains({.5,.5}));

    Plane p3({1,1}, Plane::gt, 1);
    EXPECT_FALSE(p3.contains({0,0}));
    EXPECT_TRUE(p3.contains({2,2}));
    EXPECT_FALSE(p3.contains({1,0}));
    EXPECT_FALSE(p3.contains({0,1}));
    EXPECT_FALSE(p3.contains({.5,.5}));
}

TEST(TestHET, Plane2)
{
    Conjunction s = non_negative_space(2);
    Plane p({1,1}, Plane::lte, 1);
    s.add_plane(p);

    EXPECT_TRUE(s.contains({0,0}));
    EXPECT_TRUE(s.contains({1,0}));
    EXPECT_TRUE(s.contains({0,1}));
    EXPECT_FALSE(s.contains({1,1}));

    EXPECT_TRUE(s.contains({.5,.5}));
}

TEST(TestHET, Disjunctive)
{
    Plane p1({1, .5}, Plane::lte, .5); 
    Plane p2({.5, 1}, Plane::lte, .5);
    point_t point = {.5, .5};   

    EXPECT_FALSE(p1.contains(point));
    EXPECT_FALSE(p2.contains(point));

    DisjunctionSet s1(2);
    s1.add_constraint(p1);
    s1.add_constraint(p2);
    Conjunction nn = non_negative_space(2);
    ConjunctionSet s(2);
    s.add_constraint(nn);
    s.add_constraint(s1);

    EXPECT_TRUE(s.contains({0,0}));
    EXPECT_TRUE(s.contains({1,0}));
    EXPECT_TRUE(s.contains({0,1}));
    EXPECT_FALSE(s.contains({1,1}));
    EXPECT_FALSE(s.contains({.5,.5}));
    EXPECT_TRUE(s.contains({.25,.25}));
}


TEST(TestHET, HyperplanePaper_Example1)
{
    Plane p1({1, 0, 0}, Plane::lte, 3);
    Plane p2({6, 1, 0}, Plane::lte, 18);
    Plane p3({7, 1, 0}, Plane::lte, 20);
    DisjunctionSet s1(3);
    s1.add_constraint(p2);
    s1.add_constraint(p3);

    Plane p4({0, 0, 1}, Plane::lte, 0);
    Plane p5({2, 1, 1}, Plane::lte, 6);
    Plane p6({3, 1, 1}, Plane::lte, 8);

    DisjunctionSet s2(3);
    s2.add_constraint(p4);
    s2.add_constraint(p5);
    s2.add_constraint(p6);
    
    
    ConjunctionSet space(3); 
    Conjunction nn = non_negative_space(3);
    space.add_constraint(nn);
    space.add_constraint(p1);
    space.add_constraint(s1);
    space.add_constraint(s2);

    EXPECT_TRUE(space.contains({1,1,1}));
    EXPECT_TRUE(space.contains({3,0,0}));
    EXPECT_TRUE(space.contains({0,8,0}));
    EXPECT_TRUE(space.contains({0,0,8}));
    EXPECT_FALSE(space.contains({2,2,2}));
}


TEST(TestHET, GeneratePointsExample1)
{
    vector<Task> tasks;
    tasks.push_back({1,3,3});
    tasks.push_back({1,20,20});
    tasks.push_back({1,8,8});

    vector<vector<double>> q;
    q.push_back({3});
    q.push_back({18,20});
    q.push_back({0,6,8});

    for (unsigned i =0; i<tasks.size(); ++i) {
        vector<double> p = compute_points(tasks, i, tasks[i].get_dline());
        EXPECT_EQ(q[i].size(), p.size());
        EXPECT_EQ(q[i], p);
    }

    ConjunctionSet space = create_space(tasks);
    EXPECT_EQ(4, space.size());
    EXPECT_TRUE(space.contains({1,1,1}));
    EXPECT_TRUE(space.contains({3,0,0}));
    EXPECT_TRUE(space.contains({0,8,0}));
    EXPECT_TRUE(space.contains({0,0,8}));
    EXPECT_FALSE(space.contains({2,2,2}));
}

TEST(TestHET, GeneratePointsExample2)
{
    vector<Task> tasks;
    tasks.push_back({1,3,3});
    tasks.push_back({1,7,8});
    tasks.push_back({1,19,20});

    vector<vector<double>> q;
    q.push_back({3});
    q.push_back({0,6,7});
    q.push_back({0,15,16,18,19});

    for (unsigned i =0; i<tasks.size(); ++i) {
        vector<double> p = compute_points(tasks, i, tasks[i].get_dline());
        EXPECT_EQ(q[i].size(), p.size());
        EXPECT_EQ(q[i], p);
    }

    ConjunctionSet space = create_space(tasks);
    EXPECT_EQ(4, space.size());
    EXPECT_TRUE(space.contains({3,0,0}));
    EXPECT_TRUE(space.contains({0,6,0}));
    EXPECT_TRUE(space.contains({0,0,15}));
    EXPECT_TRUE(space.contains({2,1,3}));
    EXPECT_FALSE(space.contains({2.1,1,3}));
}


TEST(TestHET, FMEtest1)
{
    Conjunction space = non_negative_space(2);
    Plane p1({.5,1}, Plane::lte, .5);
    Plane p2({1,.5}, Plane::lte, .5);
    space.add_plane(p1);
    space.add_plane(p2);

    Plane p3({1, 1}, Plane::gte, 1);
    space.add_plane(p3);

    EXPECT_FALSE(fme_is_feasible(space));
}

TEST(TestHET, FMEtest2)
{
    Conjunction space = non_negative_space(2);
    Plane p1({.5,1}, Plane::lte, .5);
    Plane p2({1,.5}, Plane::lte, .5);
    space.add_plane(p1);
    space.add_plane(p2);

    Plane p3({1, 1}, Plane::gte, .5);
    space.add_plane(p3);

    EXPECT_TRUE(fme_is_feasible(space));
}


