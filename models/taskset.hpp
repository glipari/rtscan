#ifndef __TASK_SET_H__
#define __TASK_SET_H__

#include <vector>
#include <map>
#include <algorithm>

#include <common/exceptions.hpp>
#include <models/task.hpp>

namespace Scan {
    class TaskSet {
        typedef std::vector<Task> TLIST;
        TLIST s;
    public:
        TaskSet();
        ~TaskSet();

        TaskSet(const TaskSet &ts);

        size_t size() const { return s.size(); }

        const Task& at(unsigned int i) const throw(IndexOutOfBound)
            {
                if (i >= size())
                    throw IndexOutOfBound("Index out of bound in accessing task set");
                return s[i];
            }

        Task& operator[](unsigned int i) throw (IndexOutOfBound);
        Task& operator[](const std::string &name) throw (TaskDoesNotExist);

        TaskSet& operator=(const TaskSet &ts);
        TaskSet& operator+=(const Task &t);
        TaskSet& operator+=(const TaskSet &t);

        std::vector<int> get_periods() const;
        std::vector<double> get_dlines() const;
        std::vector<double> get_wcets() const;

        double get_util() const;

        template<class Cmp>
        void sort();

        template<class Type>
        void sort_property(const std::string &name);
    };

    TaskSet operator+(const TaskSet &ts1, const TaskSet &ts2);

    class TaskCmpPeriod : public std::less<Task> {
    public:
        bool operator()(const Task &a, const Task &b) {
            return a.get_period() < b.get_period();
        }
    };

    class TaskCmpDline : public std::less<Task> {
    public:
        bool operator()(const Task &a, const Task &b) {
            return a.get_dline() < b.get_dline();
        }
    };

    class TaskCmpName : public std::less<Task> {
    public:
        bool operator()(const Task &a, const Task &b) {
            return a.get_name() < b.get_name();
        }
    };

    template<class Cmp>
    void TaskSet::sort()
    {
        std::sort(s.begin(),s.end(), Cmp());
    }
}

#endif
