#include <string>

#include <models/taskset.hpp>

namespace Scan {

    using namespace std;

    TaskSet::TaskSet() : s()
    {
    }

    TaskSet::~TaskSet() 
    {
    }

    TaskSet::TaskSet(const TaskSet &ts) 
    {
        s = ts.s;
        //m = ts.m;
    }
    
    TaskSet& TaskSet::operator=(const TaskSet &ts) 
    {
        s = ts.s;
        //m = ts.m;
        return *this;
    }

    TaskSet& TaskSet::operator+=(const Task &t)
    {
        s.push_back(t);
        //m[t.get_name()] = &(s[k]);
        return *this;
    }

    TaskSet& TaskSet::operator+=(const TaskSet &ts)
    {
        for (unsigned int i=0; i < ts.size(); ++i) {
            s.push_back(ts.at(i));
            //m[ts.at(i).get_name()] = &(s[k+i]);
        } 
        return *this;
    }
    
	Task& TaskSet::operator[](unsigned int i) throw (IndexOutOfBound) {
		if (i >= size())
			throw IndexOutOfBound("Index out of bound in accessing task set");
		return s[i];
	}

	Task& TaskSet::operator[](const std::string &name) throw (TaskDoesNotExist) {
		for (TLIST::iterator i = s.begin(); i != s.end(); ++i)
			if (i->get_name() == name) return *i;

		throw TaskDoesNotExist("Task does not exists in task set");
//		if (m.find(name) == m.end())
//			throw TaskDoesNotExist("Task does not exists in task set");
//		return (*m[name]);
	}

	TaskSet operator+(const TaskSet &ts1, const TaskSet &ts2)
	{
		TaskSet tset(ts1);
		tset += ts2;
		return tset;
	}


    std::vector<int> TaskSet::get_periods() const
    {
        unsigned int i;
        std::vector<int> v;
        for (i=0; i<size(); ++i)
            v.push_back(s[i].get_period());

        return v;
    }
    
    std::vector<double> TaskSet::get_dlines() const
    {
        unsigned int i;
        std::vector<double> v;
        for (i=0; i<size(); ++i)
            v.push_back(s[i].get_dline());

        return v;
    }
    std::vector<double> TaskSet::get_wcets() const
    {
        unsigned int i;
        std::vector<double> v;
        for (i=0; i<size(); ++i)
            v.push_back(s[i].get_wcet());

        return v;
    }

//    DemandBoundFunction TaskSet::get_dbf() const
//    {
//    	DemandBoundFunction d;
//        unsigned int i;
//        for (i=0; i<size(); ++i)
//            d = d + DemandBoundFunction(s[i]);
//
//        return d;
//    }

    double TaskSet::get_util() const
    {
    	double u = 0.0;
		for (unsigned i=0; i<s.size(); ++i)
			u += s[i].get_wcet() / double(s[i].get_period());
		return u;
    }
}
