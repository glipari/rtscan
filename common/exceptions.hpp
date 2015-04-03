#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <string>
#include <exception>

namespace Scan {
    /**
       The basic exception class. This is just a wrapper of the
       std:exception class (probably not very useful).

       @todo should we remove it? 
     */
    class Exception : public std::exception {
        std::string where;
        std::string msg;
    public:
        Exception(const std::string &w, const std::string &m);
        virtual ~Exception() throw() {}

        std::string what();
    };

    /**
       Transforms a number into a string
    */
#define NUM_TO_STR_1(line)   #line
#define NUM_TO_STR_2(line)   NUM_TO_STR_1(line)
#define STRINGIFY(text)      #text

/// Shortcut to declare an exception derived from Exception
#define DECL_EXC(xxx) class xxx : public Scan::Exception {      \
         public: \
         xxx(const std::string &s) : Scan::Exception(STRINGIFY(xxx), s) {} \
         }
/// shortcurt to throw an exception with the right parameters
/// sss must be a constant string
#define THROW_EXC(xxx, sss) throw xxx(__FILE__ ", line " NUM_TO_STR_2(__LINE__) ": " sss)

/// debugging macros
#ifdef MYDEBUG
#define DBGFILEL      do {cout << __FILE__ "@" NUM_TO_STR_2(__LINE__) ":";} while(0)
#define DBGPRINT(xxx) do {cout << xxx;} while(0)
#define DBGPRTLN(xxx) do {cout << xxx << endl;} while(0)
#define DEBUGVAR(xxx) do {cout << " -- " STRINGIFY(xxx) " = " << xxx << endl;} while(0)
#else
#define DBGFILEL
#define DBGPRINT(xxx)
#define DBGPRTLN(xxx)
#define DEBUGVAR(xxx)
#endif

    /// Exception: receive an illegal value
    DECL_EXC(IllegalValue);

    /// Exception: expecting a non negative value.
    DECL_EXC(NegativeValue);

    /// Exception: Index out of bound in some array
    DECL_EXC(IndexOutOfBound);

    /// Exception: Addressing a non-existing task
    DECL_EXC(TaskDoesNotExist);
}

#endif
