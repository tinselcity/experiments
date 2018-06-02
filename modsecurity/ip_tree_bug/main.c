#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "msc_tree.h"
//: ----------------------------------------------------------------------------
//:
//: ----------------------------------------------------------------------------
int create_radix_tree(apr_pool_t *mp, TreeRoot **rtree, char **error_msg)
{
    *rtree = malloc(sizeof(TreeRoot));
    if (*rtree == NULL)
    {
        //*error_msg = apr_psprintf(mp, "Failed allocating " "memory to TreeRoot.");
        goto root_node_failed;
    }
    memset(*rtree, 0, sizeof(TreeRoot));

    (*rtree)->ipv4_tree = CPTCreateRadixTree(mp);
    if ((*rtree)->ipv4_tree == NULL)
    {
        //*error_msg = apr_psprintf(mp, "IPmatch: Tree initialization " "failed.");
        goto ipv4_tree_failed;
    }

    (*rtree)->ipv6_tree = CPTCreateRadixTree(mp);
    if ((*rtree)->ipv6_tree == NULL)
    {
        //*error_msg = apr_psprintf(mp, "IPmatch: Tree initialization " "failed.");
        goto ipv6_tree_failed;
    }

    return 0;

ipv6_tree_failed:
ipv4_tree_failed:
root_node_failed:
    return -1;
}
//: ----------------------------------------------------------------------------
//:
//: ----------------------------------------------------------------------------
int tree_contains_ip(apr_pool_t *mp, TreeRoot *rtree,
    const char *value, modsec_rec *msr, char **error_msg)
{
    struct in_addr in;
    struct in6_addr in6;

    if (rtree == NULL)
    {
        return 0;
    }

    if (strchr(value, ':') == NULL) {
        if (inet_pton(AF_INET, value, &in) <= 0) {
            //*error_msg = apr_psprintf(mp, "IPmatch: bad IPv4 " "specification \"%s\".", value);
            return -1;
        }

        if (CPTIpMatch(msr, (unsigned char *)&in.s_addr, rtree->ipv4_tree, IPV4_TREE) != NULL) {
            return 1;
        }
    }
    else {
        if (inet_pton(AF_INET6, value, &in6) <= 0) {
            //*error_msg = apr_psprintf(mp, "IPmatch: bad IPv6 " "specification \"%s\".", value);
            return -1;
        }

        if (CPTIpMatch(msr, (unsigned char *)&in6.s6_addr, rtree->ipv6_tree, IPV6_TREE) != NULL) {
            return 1;
        }
    }

    return 0;
}
//: ----------------------------------------------------------------------------
//:
//: ----------------------------------------------------------------------------
int main(void)
{
         printf("Hello world\n");
         char *l_err_msg;
         TreeRoot *l_tree;
         apr_pool_t l_mp;
         modsec_rec l_msr;
         int l_s;
         l_s = create_radix_tree(&l_mp, &l_tree, &l_err_msg);
         printf("create_radix_tree: %d\n", l_s);
         // add "192.168.100.0/24"
         TreeNode *tnode = NULL;
         tnode = TreeAddIP("192.168.100.0/24", l_tree->ipv4_tree, IPV4_TREE);
         printf("tnode:             %p\n", tnode);
         l_s = tree_contains_ip(&l_mp, l_tree, "192.168.100.0", &l_msr, &l_err_msg);
         printf("tree_contains_ip:  %d\n", l_s);
         //char *l_buf;
         //*l_buf = 'c';
         return 0;
}
