//: ----------------------------------------------------------------------------
//: Name        : check_for_pod.hpp
//: Author      : David Andrews / Reed Morrison
//: Version     : NA
//: Copyright   : NA
//: Description : Macro to check if class is POD type data.
//: ----------------------------------------------------------------------------
#ifndef _CHECK_FOR_POD_HPP
#define _CHECK_FOR_POD_HPP

// Includes
#include <stdarg.h>

// Definitions

// Define the macro to ensure a class is POD
inline int check_for_pod(int count, ...)
{
        va_list ap;
        va_start(ap, count);
        va_end(ap);
        return 0;
};

#define CHECK_FOR_POD(_class)          \
        if(0){                         \
                _class var;            \
                check_for_pod(1, var); \
        }

#endif // _CHECK_FOR_POD_HPP


