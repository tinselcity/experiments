//: ----------------------------------------------------------------------------
//: Name        : check_for_pod.cpp
//: Author      : David Andrews / Reed Morrison
//: Version     : NA
//: Copyright   : NA
//: Description : POD Check example
//: ----------------------------------------------------------------------------

// Includes
#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "check_for_pod.hpp"

//: ----------------------------------------------------------------------------
//: Types...
//: ----------------------------------------------------------------------------
class my_cool_pod_class {

    int m_a;
    int m_b;
    int m_c;

};


class my_cool_non_pod_class {

    int m_a;
    int m_b;
    int m_c;

    std::string m_str;

};

//: ----------------------------------------------------------------------------
//: Main...
//: ----------------------------------------------------------------------------
int main(void)
{

    CHECK_FOR_POD(my_cool_pod_class);
    CHECK_FOR_POD(my_cool_non_pod_class);

    return 0;

}



