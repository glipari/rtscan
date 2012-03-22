/*
 * TestBasicFunctions.cpp
 *
 *  Created on: Jun 6, 2011
 *      Author: lipari
 */

#include "gtest/gtest.h"
#include <memory>

#include <common/functions.hpp>

using namespace Scan::function;
using namespace std;

TEST(BasicFunction, Step)
{
	PiecewiseFunction ls = lstep_(x_ - 5);
	EXPECT_EQ(0.0, ls(0));
	EXPECT_EQ(0.0, ls(5));
	EXPECT_EQ(1.0, ls(6));

	PiecewiseFunction rs = step_(x_ - 5);
	EXPECT_EQ(0.0, rs(0));
	EXPECT_EQ(1.0, rs(5));
	EXPECT_EQ(1.0, rs(6));
}


TEST(BasicFunction, Linear)
{
	PiecewiseFunction lf = x_ - 1.0;
	EXPECT_EQ(-1.0, lf(0));
	EXPECT_EQ(0.0, lf(1));
	EXPECT_EQ(1.0, lf(2));
	EXPECT_EQ(100.0, lf(101));
}

TEST(BasicFunction, StepLinear)
{
	PiecewiseFunction mf = max(0.0, x_ - 1);

	EXPECT_EQ(0.0, mf(0));
	EXPECT_EQ(0.0, mf(1));
	EXPECT_EQ(1.0, mf(2));
}

TEST(BasicFunction, Composed)
{
	PiecewiseFunction f = 2*x_;
	PiecewiseFunction h = f(x_ - 1);

	EXPECT_EQ(-2.0, h(0));
	EXPECT_EQ(0.0, h(1));
	EXPECT_EQ(2.0, h(2));
	EXPECT_EQ(3.0, h(2.5));
}


TEST(BasicFunction, xvalue)
{
	XValue x1;
	XValue x2(0);
	XValue x3(0,-1);
	XValue x4(0,+1);
	XValue x5(1.0,0);

	EXPECT_TRUE(x1 == x2);
	EXPECT_FALSE(x1 < x2);
	EXPECT_FALSE(x2 < x3);
	EXPECT_TRUE(x3 < x2);
	EXPECT_TRUE(x2 < x4);
	EXPECT_FALSE(x4 < x1);
	EXPECT_TRUE(x4 < x5);
	EXPECT_FALSE(x5 < x1);

	EXPECT_TRUE(infneg.is_infneg());
	EXPECT_TRUE(infpos.is_infpos());

	EXPECT_TRUE(infneg < x1);
	EXPECT_TRUE(x4 < infpos);
	EXPECT_TRUE(infneg<infpos);
	EXPECT_FALSE(infpos<infneg);
	EXPECT_FALSE(infpos<x1);
	EXPECT_FALSE(x4<infneg);
}
