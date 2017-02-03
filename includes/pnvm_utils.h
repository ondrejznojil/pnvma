/******************************************************************************
 * Interface for pnvm plugin not related to dynamic data types.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */
#ifndef PNVM_UTILS_H
#define PNVM_UTILS_H

#if !defined(TARGET_ARDUINO) && DEBUG
    #include <sstream>
#endif
#include "pnvm_config.h"

template<typename T>
T const * makePointer(T const & o) { return &o; }
template<typename T>
T * makePointer(T & o) { return &o; }

template<typename T>
T const & dereference(T const *o) { return *o; }
template<typename T>
T & dereference(T *o) { return *o; }

#if defined(TARGET_ARDUINO)
    #define LOG4CXX_TRACE(...)
    #define LOG4CXX_DEBUG(...)
    #define LOG4CXX_INFO(...)
    #define LOG4CXX_WARN(...)
    #define LOG4CXX_ERROR(...)
    #define LOG4CXX_FATAL(...)
#else
    #include <log4cxx/logger.h>
#endif

#if defined(TARGET_ARDUINO)
    #define dieWithmsg(code, msg) \
        dieWith(code)	// defined in pnvm_main.h
    #define dieWithmsgwith(code, msg, obj) \
        dieWithmsg(code, msg)
    #define dieWithmsgwithwith(code, msg, obj1, obj2) \
        dieWithmsgwith(code, msg, obj1)
    #define unlessdieWithmsg(cond, code, msg) \
        ((cond) ? true : setError(code) == ErrorSuccess)
    #define unlessdieWithmsgwith(cond, code, msg, obj) \
        unlessdieWithmsg(code, code, msg)
    #define unlessdieWithmsgwithwith(cond, code, msg, obj1, obj2) \
        unlessdieWithmsgwith(code, msg, obj1)
#else
    void dieWithmsg(int errcode, char const *msg);

    template <typename T1, typename T2>
    inline void dieWithmsgwithwith(
            int errcode,
            char const *msg,
            T1 const &obj1,
            T2 const &obj2)
    {
        std::ostringstream s;
        s << msg << obj1 << obj2;
        dieWithmsg(errcode, s.str().c_str());
    }

    template <typename T>
    inline void dieWithmsgwith(
            int errcode,
            char const *msg,
            T const &obj)
    {
        dieWithmsgwithwith(errcode, msg, obj, "");
    }

    inline bool unlessdieWithmsg(bool cond, int errcode, char const *msg) {
        if (!cond)
            dieWithmsg(errcode, msg);
        return cond;
    }

    template <typename T1, typename T2>
    inline bool unlessdieWithmsgwithwith(
            bool cond,
            int errcode,
            char const *msg,
            T1 const &obj1,
            T2 const &obj2)
    {
        if (!cond)
            dieWithmsgwithwith(errcode, msg, obj1, obj2);
        return cond;
    }

    template <typename T>
    inline bool unlessdieWithmsgwith(
            bool cond,
            int errcode,
            char const *msg,
            T const &obj)
    {
        unlessdieWithmsgwithwith(cond, errcode, msg, obj, "");
        return cond;
    }

    std::string asPlainString(PNVMStringHead const *str);
    
#endif

#endif /* end of include guard: PNVM_UTILS_H */
