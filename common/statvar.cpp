#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/math/distributions/students_t.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/ref.hpp>
#include <algorithm>
#include <cassert>

#include <common/statvar.hpp>

using namespace boost::accumulators;
using namespace boost::lambda;

namespace m=boost::math;

using namespace std; 

namespace Scan {

    std::vector<StatVar*> StatVar::repos;
    std::string StatVar::directory;

    StatVar::StatVar(const string &n) : name(n), values(), outfile(0)
    {
        repos.push_back(this);
        fs::path my_dir(directory);
        if (!exists(my_dir)) fs::create_directory(my_dir);
        fs::path my_path = my_dir / name;

        outfile = new fs::fstream(my_path, ios::out);
    }

    StatVar::~StatVar()
    {
        if (outfile) outfile->close();
        delete outfile;
    }

    void StatVar::add_sample(double v)
    {
        values.push_back(v);
    }

    double StatVar::avg() const
    {
        accumulator_set< double, features< tag::mean > > acc;

        std::for_each( values.begin(), values.end(),  boost::lambda::bind( boost::ref(acc), _1 ) );

        return mean(acc);
    }

    double StatVar::var() const
    {
        accumulator_set< double, features< tag::variance > > acc;

        std::for_each( values.begin(), values.end(), boost::lambda::bind( boost::ref(acc), _1 ) );

        return variance(acc);
    }

    double StatVar::conf_interval(double conf) const
    {
        assert(conf < 1 && conf > 0);

        double Sd = var();

        int Sn = values.size();

        m::students_t dist(Sn - 1);
        double T = m::quantile(m::complement(dist, (1 - conf) / 2));

        double w = T * Sd / sqrt(double(Sn));

        return w;
    }

    void StatVar::dump_data(double d)
    {
        (*outfile) << d << ", ";
    }

    void StatVar::dump_obj(double c)
    {
        (*outfile) << avg() << ", " << var() << ", "
                   << conf_interval(c) << endl;

        values.clear();
    }

    void StatVar::dump_all(double c)
    {
        for_each(repos.begin(), repos.end(),
                 boost::lambda::bind(&StatVar::dump_obj, _1, c));
    }

    void StatVar::dump_param(double par1)
    {
        for_each(repos.begin(), repos.end(),
                 boost::lambda::bind(&StatVar::dump_data, _1, par1));
    }

    void StatVar::set_dir(const std::string &dir)
    {
        directory = dir;
    }
}

