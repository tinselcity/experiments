//: ----------------------------------------------------------------------------
//: includes
//: ----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <string>
#include <set>
#include "nms.h"
#include "msc_tree.h"
#include "tinymt32.cc"
#ifdef ENABLE_PROFILER
#include <gperftools/heap-profiler.h>
#include <gperftools/profiler.h>
#endif
//: ----------------------------------------------------------------------------
//: constants
//: ----------------------------------------------------------------------------
#ifndef STATUS_OK
#define STATUS_OK 0
#endif
#ifndef STATUS_ERROR
#define STATUS_ERROR -1
#endif
//#define MAX_READLINE_SIZE 1024
#define DEFAULT_SIZE 1000000
//: ----------------------------------------------------------------------------
//: macros
//: ----------------------------------------------------------------------------
#ifndef NDBG_OUTPUT
#define NDBG_OUTPUT(...) \
        do { \
                fprintf(stdout, __VA_ARGS__); \
                fflush(stdout); \
        } while(0)
#endif

#ifndef NDBG_PRINT
#define NDBG_PRINT(...) \
        do { \
                fprintf(stdout, "%s:%s.%d: ", __FILE__, __FUNCTION__, __LINE__); \
                fprintf(stdout, __VA_ARGS__);               \
                fflush(stdout); \
        } while(0)
#endif
//: ----------------------------------------------------------------------------
//: types
//: ----------------------------------------------------------------------------
typedef enum {
        DS_TYPE_MSC_TREE = 0,
        DS_TYPE_NMS
} ip_type_t;
typedef std::set<std::string> ip_set_t;
//: ----------------------------------------------------------------------------
//: create msc_tree
//: ----------------------------------------------------------------------------
int create_radix_tree(apr_pool_t *mp, TreeRoot **rtree, char **error_msg)
{
        *rtree = (TreeRoot *)malloc(sizeof(TreeRoot));
        if(*rtree == NULL)
        {
                //*error_msg = apr_psprintf(mp, "Failed allocating " "memory to TreeRoot.");
                goto root_node_failed;
        }
        memset(*rtree, 0, sizeof(TreeRoot));
        (*rtree)->ipv4_tree = CPTCreateRadixTree(mp);
        if((*rtree)->ipv4_tree == NULL)
        {
                //*error_msg = apr_psprintf(mp, "IPmatch: Tree initialization " "failed.");
                goto ipv4_tree_failed;
        }
        (*rtree)->ipv6_tree = CPTCreateRadixTree(mp);
        if((*rtree)->ipv6_tree == NULL)
        {
                //*error_msg = apr_psprintf(mp, "IPmatch: Tree initialization " "failed.");
                goto ipv6_tree_failed;
        }
        return STATUS_OK;
ipv6_tree_failed:
ipv4_tree_failed:
root_node_failed:
        return STATUS_ERROR;
}
//: ----------------------------------------------------------------------------
//:
//: ----------------------------------------------------------------------------
int tree_contains_ip(apr_pool_t *mp,
                     TreeRoot *rtree,
                     const char *value,
                     modsec_rec *msr,
                     char **error_msg)
{
        struct in_addr in;
        struct in6_addr in6;
        if(rtree == NULL)
        {
                return 0;
        }
        if(strchr(value, ':') == NULL)
        {
                if(inet_pton(AF_INET, value, &in) <= 0)
                {
                        //*error_msg = apr_psprintf(mp, "IPmatch: bad IPv4 " "specification \"%s\".", value);
                        return -1;
                }
                if(CPTIpMatch(msr, (unsigned char *)&in.s_addr, rtree->ipv4_tree, IPV4_TREE) != NULL)
                {
                        return 1;
                }
        }
        else
        {
                if(inet_pton(AF_INET6, value, &in6) <= 0)
                {
                        //*error_msg = apr_psprintf(mp, "IPmatch: bad IPv6 " "specification \"%s\".", value);
                        return -1;
                }
                if(CPTIpMatch(msr, (unsigned char *)&in6.s6_addr, rtree->ipv6_tree, IPV6_TREE) != NULL)
                {
                        return 1;
                }
        }
        return 0;
}
//: ----------------------------------------------------------------------------
//: \details:  TODO
//: \return:   TODO
//: \param:    TODO
//: ----------------------------------------------------------------------------
void print_usage(FILE* a_stream, int a_exit_code)
{
        fprintf(a_stream, "Usage: ip_tree [OPTIONS]\n");
        fprintf(a_stream, "IP Tree Test.\n");
        fprintf(a_stream, "\n");
        fprintf(a_stream, "OPTIONS:\n");
        fprintf(a_stream, "  -h, --help       Display this help and exit.\n");
        fprintf(a_stream, "  -s, --seed       Tiny Mersenne Twister seed  (default: 12345).\n");
        fprintf(a_stream, "  -t, --type       IP Tree type (MSC_TREE|NMS) (default: MSC_TREE).\n");
#ifdef ENABLE_PROFILER
        fprintf(a_stream, "  -H, --hprofile   Google heap profiler output file\n");
        fprintf(a_stream, "  -C, --cprofile   Google cpu profiler output file\n");
#endif
        fprintf(a_stream, "\n");
        _exit(a_exit_code);
}
//: ----------------------------------------------------------------------------
//: check file page cache state
//: ----------------------------------------------------------------------------
int main(int argc, char **argv)
{
        char l_opt;
        std::string l_arg;
        int l_option_index = 0;
        uint64_t l_tmtw_seed = 12345;
#ifdef ENABLE_PROFILER
        std::string l_hprof_file;
        std::string l_cprof_file;
#endif
        ip_type_t l_type = DS_TYPE_MSC_TREE;
        uint32_t l_size = DEFAULT_SIZE;
        struct option l_long_options[] =
                {
                { "help",         0, 0, 'h' },
                { "seed",         1, 0, 's' },
                { "type",         1, 0, 't' },
#ifdef ENABLE_PROFILER
                { "cprofile",     1, 0, 'H' },
                { "hprofile",     1, 0, 'C' },
#endif
                // list sentinel
                { 0, 0, 0, 0 }
        };
#ifdef ENABLE_PROFILER
        while ((l_opt = getopt_long_only(argc, argv, "hs:t:H:C:", l_long_options, &l_option_index)) != -1)
#else
        while ((l_opt = getopt_long_only(argc, argv, "hs:t:", l_long_options, &l_option_index)) != -1)
#endif
        {
                if(optarg)
                        l_arg = std::string(optarg);
                else
                        l_arg.clear();

                switch (l_opt)
                {
                // -----------------------------------------
                // help
                // -----------------------------------------
                case 'h':
                {
                        print_usage(stdout, 0);
                        break;
                }
                // -----------------------------------------
                // ?
                // -----------------------------------------
                case '?':
                {
                        // Required argument was missing
                        // '?' is provided when the 3rd arg to getopt_long does not begin with a ':', and is preceeded
                        // by an automatic error message.
                        fprintf(stdout, "  Exiting.\n");
                        print_usage(stdout, -1);
                        break;
                }
                // -----------------------------------------
                // seed
                // -----------------------------------------
                case 's':
                {
                        l_tmtw_seed = strtoul(l_arg.c_str(), NULL, 10);
                        break;
                }
                // -----------------------------------------
                // type
                // -----------------------------------------
                case 't':
                {
                        if(l_arg == "MSC_TREE")
                        {
                                l_type = DS_TYPE_MSC_TREE;
                        }
                        else if(l_arg == "NMS")
                        {
                                l_type = DS_TYPE_NMS;
                        }
                        else
                        {
                                fprintf(stdout, "Unrecognized type option: %s\n", l_arg.c_str());
                                print_usage(stdout, -1);
                        }
                        break;
                }
                // -----------------------------------------
                // profiler file
                // -----------------------------------------
#ifdef ENABLE_PROFILER
                case 'H':
                {
                        l_hprof_file = l_arg;
                        break;
                }
#endif
                // -----------------------------------------
                // profiler file
                // -----------------------------------------
#ifdef ENABLE_PROFILER
                case 'C':
                {
                        l_cprof_file = l_arg;
                        break;
                }
#endif
                // -----------------------------------------
                // ?
                // -----------------------------------------
                default:
                {
                        fprintf(stdout, "Unrecognized option.\n");
                        print_usage(stdout, -1);
                        break;
                }
                }
        }
        // -------------------------------------------------
        // seed twister
        // -------------------------------------------------
        tinymt32_t l_tmtw;
        tinymt32_init(&l_tmtw, l_tmtw_seed);
        // -------------------------------------------------
        // generate ip list
        // -------------------------------------------------
        printf(": generating ip list...\n");
        ip_set_t l_ip_set;
        for(uint32_t i_ip = 0; i_ip <= l_size; ++i_ip)
        {
                uint64_t l_ip;
                l_ip = tinymt32_generate_uint32(&l_tmtw);
                uint32_t l_ipv4;
                l_ipv4 = (uint32_t)(l_ip);
                struct in_addr ip_addr;
                ip_addr.s_addr = l_ipv4;
                const char *l_ip_str;
                l_ip_str = inet_ntoa(ip_addr);
                l_ip_set.insert(l_ip_str);
        }
        printf(": generated ip list of size: %lu\n", l_ip_set.size());
        // -------------------------------------------------
        // print sample
        // -------------------------------------------------
#if 0
        printf(": printing sample...\n");
        uint32_t l_display_num = 10;
        uint32_t i_display = 0;
        for(ip_set_t::iterator i_ip = l_ip_set.begin();; ++i_ip)
        {
                printf(": [%u]: %s\n", i_display, i_ip->c_str());
                ++i_display;
                if(i_display >= l_display_num)
                {
                        break;
                }
        }
#endif
        // -------------------------------------------------
        // creating tree
        // -------------------------------------------------
        struct mallinfo l_mallinfo;
        l_mallinfo = mallinfo();
        uint32_t l_bytes_allocd_begin;
        l_bytes_allocd_begin = l_mallinfo.uordblks;
#if 0
        printf(": mallinfo.arena:    %d\n", l_mallinfo.arena);
        printf(": mallinfo.fordblks: %d\n", l_mallinfo.fordblks);
        printf(": mallinfo.fsmblks:  %d\n", l_mallinfo.fsmblks);
        printf(": mallinfo.hblkhd:   %d\n", l_mallinfo.hblkhd);
        printf(": mallinfo.hblks:    %d\n", l_mallinfo.hblks);
        printf(": mallinfo.keepcost: %d\n", l_mallinfo.keepcost);
        printf(": mallinfo.ordblks:  %d\n", l_mallinfo.ordblks);
        printf(": mallinfo.smblks:   %d\n", l_mallinfo.smblks);
        printf(": mallinfo.uordblks: %d\n", l_mallinfo.uordblks);
        printf(": mallinfo.usmblks:  %d\n", l_mallinfo.usmblks);
#endif
        printf(": populating data...\n");
        // msc_tree only...
        char *l_err_msg;
        TreeRoot *l_tree;
        apr_pool_t l_mp;
        // waflz only
        ns_waflz::nms *l_nms = NULL;
        switch(l_type) {
        // -------------------------------------------------
        // MSC_TREE
        // -------------------------------------------------
        case(DS_TYPE_MSC_TREE):
        {
                int l_s;
                l_s = create_radix_tree(&l_mp, &l_tree, &l_err_msg);
                (void)l_s;
                //printf(": create_radix_tree: %d\n", l_s);
                for(ip_set_t::iterator i_ip = l_ip_set.begin(); i_ip != l_ip_set.end(); ++i_ip)
                {
                        //modsec_rec l_msr;
                        TreeNode *tnode = NULL;
                        tnode = TreeAddIP(i_ip->c_str(), l_tree->ipv4_tree, IPV4_TREE);
                        (void)tnode;
                        //printf(": tnode:             %p\n", tnode);
                        //l_s = tree_contains_ip(&l_mp, l_tree, i_ip->c_str(), &l_msr, &l_err_msg);
                        //printf(": tree_contains_ip:  %d\n", l_s);
                }
                break;
        }
        // -------------------------------------------------
        // NMS
        // -------------------------------------------------
        case(DS_TYPE_NMS):
        {
                l_nms = new ns_waflz::nms();
                for(ip_set_t::iterator i_ip = l_ip_set.begin(); i_ip != l_ip_set.end(); ++i_ip)
                {
                        l_nms->add(i_ip->c_str(), i_ip->length());
                }
                break;
        }
        // -------------------------------------------------
        // default
        // -------------------------------------------------
        default:
        {
                break;
        }
        }
        // -------------------------------------------------
        // search
        // -------------------------------------------------
#if 0
        uint32_t l_search_num = 10;
        uint32_t i_search = 0;
        for(ip_set_t::iterator i_ip = l_ip_set.begin();; ++i_ip)
        {
                printf(": [%u]: %s\n", i_search, i_ip->c_str());
                ++i_search;
                if(i_search >= l_search_num)
                {
                        break;
                }
                modsec_rec l_msr;
                int32_t l_s;
                l_s = tree_contains_ip(&l_mp, l_tree, i_ip->c_str(), &l_msr, &l_err_msg);
                printf(": tree_contains_ip:  %d\n", l_s);
        }
#endif
        // -------------------------------------------------
        // get size used
        // -------------------------------------------------
        l_mallinfo = mallinfo();
        uint32_t l_bytes_allocd_end;
        l_bytes_allocd_end = l_mallinfo.uordblks - l_bytes_allocd_begin;
        printf(": used: %d kB...\n", l_bytes_allocd_end / 1024);
        // -------------------------------------------------
        // cleanup
        // -------------------------------------------------
        printf(": cleaning up...\n");
        if(l_nms) { delete l_nms; l_nms = NULL;}
        return STATUS_OK;
}
#if 0

//: ----------------------------------------------------------------------------
//: \details:  TODO
//: \return:   TODO
//: \param:    TODO
//: ----------------------------------------------------------------------------
int main(int argc, char **argv)
{
        if(l_file_str.empty())
        {
                NDBG_OUTPUT("Error no input provided\n");
                print_usage(stdout, -1);
        }
        FILE * l_fp;
        int32_t l_s = STATUS_OK;
        l_fp = fopen(l_file_str.c_str(),"r");
        if(NULL == l_fp)
        {
                NDBG_OUTPUT("Error opening file: %s.  Reason: %s\n", l_file_str.c_str(), strerror(errno));
                return STATUS_ERROR;
        }
        char l_readline[MAX_READLINE_SIZE];
#ifdef ENABLE_PROFILER
        if(!l_hprof_file.empty())
        {
                HeapProfilerStart(l_hprof_file.c_str());
        }
#endif
        // -------------------------------------------------
        // init...
        // -------------------------------------------------
        ns_waflz::ip_tree *l_ip_tree = NULL;
        ns_waflz::nms *l_nms = NULL;
        netmask_set *l_netmask_set = NULL;
        switch(l_type)
        {
        case DS_TYPE_IPTREE:
        {
                l_ip_tree = new ns_waflz::ip_tree();
                break;
        }
        case DS_TYPE_NETMASK_SET:
        {
                l_netmask_set = new netmask_set();
                break;
        }
        case DS_TYPE_NMS:
        {
                l_nms = new ns_waflz::nms();
                break;
        }
        default: { break;}
        }
        // -------------------------------------------------
        // readline
        // -------------------------------------------------
        while(fgets(l_readline, sizeof(l_readline), l_fp))
        {
                size_t l_readline_len = strnlen(l_readline, MAX_READLINE_SIZE);
                if(MAX_READLINE_SIZE == l_readline_len)
                {
                        // line was truncated
                        // Bail out -reject lines longer than limit
                        // -host names ought not be too long
                        NDBG_OUTPUT("Error: lines must be shorter then %d chars\n", MAX_READLINE_SIZE);
                        return STATUS_ERROR;
                }
                // read full line
                // Nuke endline
                l_readline[l_readline_len - 1] = '\0';
                //std::string l_line;
                //l_line.assign(l_readline);
                //l_line.erase( std::remove_if( l_line.begin(), l_line.end(), ::isspace ), l_line.end() );
                //if(l_line.empty())
                //{
                //        continue;
                //}
                //NDBG_PRINT("READLINE: %s\n", l_line.c_str());
                switch(l_type)
                {
                case DS_TYPE_IPTREE:
                {
                        l_ip_tree->add_ip(l_readline, l_readline_len - 1);
                        break;
                }
                case DS_TYPE_NETMASK_SET:
                {
                        l_netmask_set->add(l_readline, l_readline_len - 1, true);
                        break;
                }
                case DS_TYPE_NMS:
                {
                        l_nms->add(l_readline, l_readline_len - 1);
                        break;
                }
                default: { break;}
                }
        }
#ifdef ENABLE_PROFILER
        if(!l_hprof_file.empty())
        {
                HeapProfilerStop();
        }
#endif
        //printf("ADD_FILE: DONE: %s\n", a_url_file.c_str());
        l_s = fclose(l_fp);
        if(0 != l_s)
        {
                NDBG_OUTPUT("Error performing fclose.  Reason: %s\n", strerror(errno));
                return STATUS_ERROR;
        }
        NDBG_PRINT("ADD_FILE: DONE:\n");
        // -------------------------------------------------
        // display
        // -------------------------------------------------
        // TODO
        // -------------------------------------------------
        // vectors
        // -------------------------------------------------
        const char * const l_search_v[] = {
                "89.70.217.98",
                "176.10.99.204",
                "46.166.188.222",
                "176.10.99.203",
                "65.19.167.132"
        };
        const uint32_t l_search_l[] = {
                sizeof("89.70.217.98"),
                sizeof("176.10.99.204"),
                sizeof("46.166.188.222"),
                sizeof("176.10.99.203"),
                sizeof("65.19.167.132")
        };
        // -------------------------------------------------
        // search
        // -------------------------------------------------
#ifdef ENABLE_PROFILER
        if(!l_cprof_file.empty())
        {
                ProfilerStart(l_cprof_file.c_str());
        }
#endif
        ///for(uint32_t i = 0; i < 20000000; ++i)
        for(uint32_t i = 0; i < 100000; ++i)
        {
                //NDBG_PRINT("TRY_MATCH: %s\n", l_search_v[i]);
                const char *l_search = l_search_v[i%5];
                uint32_t l_size = l_search_l[i%5];
                switch(l_type)
                {
                case DS_TYPE_IPTREE:
                {
                        bool l_m = false;
                        void *l_p = NULL;
                        l_p = l_ip_tree->find_ip(l_search, l_size);
                        if(l_p) { l_m = true; }
                        //NDBG_PRINT("match: %d\n", l_m);
                        (void)l_m;
                        break;
                }
                case DS_TYPE_NETMASK_SET:
                {
                        int l_s;
                        bool l_m = false;
                        if(memchr(l_search, ':', l_size) == NULL)
                        {
                                struct in_addr l_in;
                                l_s = inet_pton(AF_INET, l_search, &l_in);
                                if(l_s <= 0)
                                {
                                        exit(-1);
                                }
                                //printf("%s.%s.%d: checking = 0x%08X %3d.%3d.%3d.%3d\n",
                                //                __FILE__,__FUNCTION__,__LINE__,
                                //                l_in.s_addr,
                                //                (int)(unsigned char)((0xFF       & l_in.s_addr) >> 0),
                                //                (int)(unsigned char)((0xFF00     & l_in.s_addr) >> 8),
                                //                (int)(unsigned char)((0xFF0000   & l_in.s_addr) >> 16),
                                //                (int)(unsigned char)((0xFF000000 & l_in.s_addr) >> 24));
                                l_m = l_netmask_set->contains_ipv4(l_in.s_addr);
                        }
                        else
                        {
                                struct in6_addr l_in6;
                                l_s = inet_pton(AF_INET6, l_search, &l_in6);
                                if(l_s <= 0)
                                {
                                        exit(-1);
                                }
                                l_m = l_netmask_set->contains_ipv6(l_in6);
                        }
                        //NDBG_PRINT("match: %d\n", l_m);
                        (void)l_m;
                        break;
                }
                case DS_TYPE_NMS:
                {
                        bool l_m = false;
                        int32_t l_s;
                        l_s = l_nms->contains(l_m, l_search, l_size);
                        //NDBG_PRINT("match: %d\n", l_m);
                        (void)l_m;
                        (void)l_s;
                        break;
                }
                default: { break;}
                }
        }
#ifdef ENABLE_PROFILER
        if(!l_cprof_file.empty())
        {
                ProfilerStop();
        }
#endif
        // -------------------------------------------------
        // cleanup
        // -------------------------------------------------
        if(l_ip_tree) { delete l_ip_tree; l_ip_tree = NULL; }
        if(l_netmask_set) { delete l_netmask_set; l_netmask_set = NULL; }
        if(l_nms) { delete l_nms; l_nms = NULL; }
        return 0;
}

#endif
