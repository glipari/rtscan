#include "catch.hpp"

#include <models/fp_task.hpp>
#include <analysis/hyperplane.hpp>

using namespace Scan;
using namespace std;

TEST_CASE("TestHET, Points1")
{
    vector<FPTask> tset = { {2, 5, 10}, {3, 12, 15} };

    vector<int> p = compute_points(tset.rbegin(), tset.rend(), 12);

    REQUIRE(2 ==  p.size());
    REQUIRE(p[0] ==  10);
    REQUIRE(p[1] ==  12);
}

TEST_CASE("TestHET, PositiveSpace")
{
    for (int n=2; n<10; n++) {
        Conjunction s = non_negative_space(n);
        for (int i=0; i<n; ++i) {
            Plane p = s.get(i);
            for (int j=0; j<n; ++j) {
                if (j != i) 
                    REQUIRE(0 ==  p.a[j]);
                else 
                    REQUIRE(1 ==  p.a[j]);
            }
            REQUIRE(Plane::gte ==  p.sign);
            REQUIRE(0 ==  p.b);
        }
    }
    
    Conjunction s1 = non_negative_space(2);
    
    CHECK(s1.is_in({1,1}));
    CHECK(s1.is_in({1,0}));
    CHECK(s1.is_in({0,1}));
    CHECK(s1.is_in({0,0}));
    CHECK_FALSE(s1.is_in({-1,1}));
    CHECK_FALSE(s1.is_in({1,-1}));
    CHECK_FALSE(s1.is_in({0,-1}));
}

TEST_CASE("TestHET, Plane1")
{
    Plane p1({1,0}, Plane::gte, 1);
    CHECK_FALSE(p1.is_in({0,0}));
    CHECK(p1.is_in({2,2}));
    CHECK(p1.is_in({1,0}));
    CHECK_FALSE(p1.is_in({0,1}));


    Plane p2({1,1}, Plane::gte, 1);
    CHECK_FALSE(p2.is_in({0,0}));
    CHECK(p2.is_in({2,2}));
    CHECK(p2.is_in({1,0}));
    CHECK(p2.is_in({0,1}));
    CHECK(p2.is_in({.5,.5}));

    Plane p3({1,1}, Plane::gt, 1);
    CHECK_FALSE(p3.is_in({0,0}));
    CHECK(p3.is_in({2,2}));
    CHECK_FALSE(p3.is_in({1,0}));
    CHECK_FALSE(p3.is_in({0,1}));
    CHECK_FALSE(p3.is_in({.5,.5}));
}

TEST_CASE("TestHET, Plane2")
{
    Conjunction s = non_negative_space(2);
    Plane p({1,1}, Plane::lte, 1);
    s.add_plane(p);

    CHECK(s.is_in({0,0}));
    CHECK(s.is_in({1,0}));
    CHECK(s.is_in({0,1}));
    CHECK_FALSE(s.is_in({1,1}));

    CHECK(s.is_in({.5,.5}));
}

TEST_CASE("TestHET, Disjunctive")
{
    Plane p1({1, .5}, Plane::lte, .5); 
    Plane p2({.5, 1}, Plane::lte, .5);
    point_t point = {.5, .5};   

    CHECK_FALSE(p1.is_in(point));
    CHECK_FALSE(p2.is_in(point));

    DisjunctionSet s1(2);
    s1.add_constraint(p1);
    s1.add_constraint(p2);
    Conjunction nn = non_negative_space(2);
    ConjunctionSet s(2);
    s.add_constraint(nn);
    s.add_constraint(s1);

    CHECK(s.is_in({0,0}));
    CHECK(s.is_in({1,0}));
    CHECK(s.is_in({0,1}));
    CHECK_FALSE(s.is_in({1,1}));
    CHECK_FALSE(s.is_in({.5,.5}));
    CHECK(s.is_in({.25,.25}));
}

TEST_CASE("TestHET, HyperplanePaper_Example1")
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

    CHECK(space.is_in({1,1,1}));
    CHECK(space.is_in({3,0,0}));
    CHECK(space.is_in({0,8,0}));
    CHECK(space.is_in({0,0,8}));
    CHECK_FALSE(space.is_in({2,2,2}));

    space.project(0,1);
    space.project(1,1);
    space.project(2,1);

    CHECK(space.is_in({10,10,10}));
}

TEST_CASE("TestHET, GeneratePointsExample1")
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
        REQUIRE(q[i].size() ==  p.size());
        REQUIRE(q[i] ==  p);
    }

    ConjunctionSet space = create_space(tasks);
    REQUIRE(4 ==  space.size());
    CHECK(space.is_in({1,1,1}));
    CHECK(space.is_in({3,0,0}));
    CHECK(space.is_in({0,8,0}));
    CHECK(space.is_in({0,0,8}));
    CHECK_FALSE(space.is_in({2,2,2}));
}

TEST_CASE("TestHET, GeneratePointsExample2")
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
        REQUIRE(q[i].size() ==  p.size());
        REQUIRE(q[i] ==  p);
    }

    ConjunctionSet space = create_space(tasks);
    REQUIRE(4 ==  space.size());
    CHECK(space.is_in({3,0,0}));
    CHECK(space.is_in({0,6,0}));
    CHECK(space.is_in({0,0,15}));
    CHECK(space.is_in({2,1,3}));
    CHECK_FALSE(space.is_in({2.1,1,3}));
}

TEST_CASE("TestHET, FMEtest1")
{
    Conjunction space = non_negative_space(2);
    Plane p1({.5,1}, Plane::lte, .5);
    Plane p2({1,.5}, Plane::lte, .5);
    space.add_plane(p1);
    space.add_plane(p2);

    Plane p3({1, 1}, Plane::gte, 1);
    space.add_plane(p3);

    CHECK_FALSE(fme_is_feasible(space));
}

TEST_CASE("TestHET, FMEtest2")
{
    Conjunction space = non_negative_space(2);
    Plane p1({.5,1}, Plane::lte, .5);
    Plane p2({1,.5}, Plane::lte, .5);
    space.add_plane(p1);
    space.add_plane(p2);

    Plane p3({1, 1}, Plane::gte, .5);
    space.add_plane(p3);

    CHECK(fme_is_feasible(space));
}

TEST_CASE("TestHET, FMEtest3")
{
    Conjunction s = non_negative_space(2);

    Plane p1({2,1}, Plane::lte, 2);
    Plane p2({1,1}, Plane::gt, 1);
    Plane p3({2,1}, Plane::lte, 1);
    Plane p4({1,2}, Plane::lte, 1);

    s.add_plane(p1); s.add_plane(p2); s.add_plane(p3); s.add_plane(p4);

    CHECK(!fme_is_feasible(s));
}


TEST_CASE("TestHET, FMEtest4")
{
    Conjunction s(2);

    Plane p1({2,1}, Plane::gt, 2);
    Plane p2({1,1}, Plane::lte, 1);
    Plane p4({1,2}, Plane::gt, 1);

    s.add_plane(p1); s.add_plane(p2); s.add_plane(p4);
    // s.normalize();
    // cout << s << endl;

    CHECK_FALSE(fme_is_feasible(s));
}

TEST_CASE("TestHET, Tighen1")
{
    Conjunction s = non_negative_space(2);

    Plane p1({2,1}, Plane::lte, 2);
    Plane p2({1,1}, Plane::lte, 1);
    Plane p3({2,1}, Plane::lte, 1);
    Plane p4({1,2}, Plane::lte, 1);
    
    s.add_plane(p1); s.add_plane(p2); s.add_plane(p3); s.add_plane(p4);

    REQUIRE(6 ==  s.size());

    s.tighen();
    
    REQUIRE(4 ==  s.size());
}

TEST_CASE("TestHET, Tighen2")
{
    Disjunction s(2);

    Plane p1({2,1}, Plane::lte, 2);
    Plane p2({1,1}, Plane::lte, 1);
    Plane p3({2,1}, Plane::lte, 1);
    Plane p4({1,2}, Plane::lte, 1);
    
    s.add_plane(p1); s.add_plane(p2); s.add_plane(p3); s.add_plane(p4);

    REQUIRE(4 ==  s.size());
    s.tighen();
    REQUIRE(2 ==  s.size());
}
