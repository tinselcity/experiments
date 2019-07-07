#include <stdio.h>
#include <set>
#include <string>
int main(void)
{
        typedef std::set<std::string> ip_set_t;
        ip_set_t l_ip_set;
        // -------------------------------------------------
        // populate
        // -------------------------------------------------
        l_ip_set.insert("127.0.0.1");
        l_ip_set.insert("1.2.3.4");
        // -------------------------------------------------
        // search
        // -------------------------------------------------
        if(l_ip_set.find("1.2.3.4") != l_ip_set.end())
        {
                printf("found it!\n");
        }
        return 0;
}
