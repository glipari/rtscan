#ifndef STATVAR_H
#define STATVAR_H

#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace Scan {

    namespace fs = boost::filesystem;

    class StatVar {
        static std::vector<StatVar*> repos;
        static std::string directory;

        std::string name;
        std::vector<double> values;

        fs::fstream *outfile;

    public:
        StatVar(const std::string &n);

        ~StatVar();

        void add_sample(double v);
        double avg() const;
        double var() const;
        double conf_interval(double conf) const;

        std::string get_name() const;

        /// @param c confidence interval
        void dump_obj(double c);

        /// @param par1 any parameter
        void dump_data(double par1);

        /// @param c confidence interval
        static void dump_all(double c);

        /// @param par1 any parameter
        static void dump_param(double par1);

        /// @param dir directory where to store all results
        static void set_dir(const std::string &dir);
    };
}

#endif
