#ifndef __HAS_SLACK_HPP__
#define __HAS_SLACK_HPP__

namespace Scan {
    class HasSlack {
    public:
        double slack;
        HasSlack() : slack(0) {}
        inline double get_slack() const { return slack; }
        inline void set_slack(double s) { slack = s; }
    };
}

#endif
