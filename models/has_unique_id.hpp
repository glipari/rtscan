#ifndef __HAS_UNIQUE_ID_HPP__
#define __HAS_UNIQUE_ID_HPP__

namespace Scan {
    class HasUniqueId {
        int id;
        static int counter;
    public:
        HasUniqueId();
        HasUniqueId(const HasUniqueId& other);
        HasUniqueId& operator=(const HasUniqueId& other);
        inline int get_id() const { return id; }
    };

    class LessUniqueId {
    public:
        bool operator()(const HasUniqueId& a, const HasUniqueId& b) const;
    };
}

#endif
