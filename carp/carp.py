#!/usr/bin/python3
# ------------------------------------------------------------------------------
# imports
# ------------------------------------------------------------------------------
import argparse
import json
import pprint
import os
import sys
import json
import ctypes
import hashlib

# ------------------------------------------------------------------------------
# constants
# ------------------------------------------------------------------------------
CARP64_END_PRIME = 7347990519673327981

# ------------------------------------------------------------------------------
# rotate left by...
# ------------------------------------------------------------------------------
def rotate_left(x, n):
    '''
    x: number to rotate
    n: bits to rotate left by
    '''
 
    return (x << n)|(x >> (64 - n))

# ------------------------------------------------------------------------------
# unsignd int hash
# ------------------------------------------------------------------------------
def uint_hash(a_str):
    '''
    return unsigned in hash
    ref: https://stackoverflow.com/a/18766695
    '''
    
    return ctypes.c_size_t(hash(a_str)).value

# ------------------------------------------------------------------------------
# combine hash
# ------------------------------------------------------------------------------
def server_hash(a_server):
    '''
    create server hash from name
    '''

    l_hash = int.from_bytes(hashlib.sha256(a_server.encode('ascii')).digest()[:8], 'little')
    return l_hash

# ------------------------------------------------------------------------------
# combine hash
# ------------------------------------------------------------------------------
def combine_hash(a_uri_hash, a_server_hash):
    '''
    combine uri and server hash
    '''

    l_hash = a_uri_hash ^ a_server_hash
    l_hash += l_hash * CARP64_END_PRIME;
    # force value back to 64 bits
    l_hash = l_hash & 0xffffffffffffffff
    l_hash = rotate_left(l_hash, 21)
    return l_hash

# ------------------------------------------------------------------------------
# calc_load_factors
# ------------------------------------------------------------------------------
def calc_load_factors(a_config):
    '''
    calculate load factors for given config
    '''

    # ------------------------------------------------------
    # create server hashes
    # ------------------------------------------------------
    l_servers = a_config['servers']
    for i_s in l_servers:
        i_s['hash'] = server_hash(i_s['name'])
    l_ixs = []
    # ------------------------------------------------------
    # create dict of:
    #   index
    #   weight
    # ------------------------------------------------------
    i_s_idx = 0
    l_w_sum = 0.0
    l_num_servers = len(l_servers)
    for i_s in l_servers:
        l_s = {}
        l_s['index'] = i_s_idx
        l_s['weight'] = i_s['weight']
        l_ixs.append(l_s)
        l_w_sum += i_s['weight']
        i_s_idx += 1
    # ------------------------------------------------------
    # sort by weight
    # ------------------------------------------------------
    l_ixs = sorted(l_ixs, key=lambda i_s: i_s['weight'])
    # ------------------------------------------------------
    # build a set of weights in P that sum to 1
    # ------------------------------------------------------
    l_p = [None] * l_num_servers
    for i_idx in range(l_num_servers):
        l_index = l_ixs[i_idx]['index']
        l_p[l_index] = l_servers[l_index]['weight'] / l_w_sum        
    # ------------------------------------------------------
    # base case
    # ------------------------------------------------------
    l_n = l_num_servers;
    l_w_idx = l_ixs[0]['index']
    l_lf = pow(l_n*l_p[l_w_idx], (1.0/l_n))
    l_servers[l_w_idx]['load_factor'] = l_lf
    l_prod = l_lf
    # ------------------------------------------------------
    # calculate
    # ------------------------------------------------------
    for i_idx in range(1,l_num_servers):
        # -----------------------------------------
        # get index
        # -----------------------------------------
        l_k_d = l_n - i_idx
        # -----------------------------------------
        # get indexes
        # -----------------------------------------
        l_w_idx = l_ixs[i_idx]['index'];
        l_w_last_idx = l_ixs[i_idx-1]['index'];
        # -----------------------------------------
        # get weights
        # -----------------------------------------
        l_w = l_p[l_w_idx];
        l_w_last = l_p[l_w_last_idx];
        # -----------------------------------------
        # get weight delta
        # -----------------------------------------
        l_w_d = l_w - l_w_last;
        # -----------------------------------------
        # calculate correction from last
        # -----------------------------------------
        l_t = (l_k_d * l_w_d) / l_prod;
        # -----------------------------------------
        # calculate scaled last load value
        # -----------------------------------------
        l_t += pow(l_servers[l_w_last_idx]['load_factor'], l_k_d);
        # -----------------------------------------
        # next load factor
        # -----------------------------------------
        l_lf = pow(l_t, (1.0/(l_k_d)))
        l_servers[l_w_idx]['load_factor'] = l_lf
        # -----------------------------------------
        # calculate next total pi value
        # -----------------------------------------
        l_prod *= l_lf;
    # ------------------------------------------------------
    # done...
    # ------------------------------------------------------
    return

# ------------------------------------------------------------------------------
# unsignd int hash
# ------------------------------------------------------------------------------
def server_carp(a_servers, a_uri):
    '''
    carp uri to a backend servers
    '''

    # ------------------------------------------------------
    # calc uri hash
    # ------------------------------------------------------
    l_uri_hash = uint_hash(a_uri)
    # ------------------------------------------------------
    # for each server
    # ------------------------------------------------------
    l_selected_idx = 0
    l_idx = 0
    l_max_hash = 0.0
    for i_s in a_servers:
        # --------------------------------------------------
        # calculate combined hash
        # --------------------------------------------------
        l_hash = combine_hash(a_uri_hash=l_uri_hash,
                              a_server_hash=i_s['hash'])
        # --------------------------------------------------
        # weight with load factor
        # NOTE this strips 11 bits of hash since
        # double only preserves 53 significand bits
        # --------------------------------------------------
        l_wtd_hash = i_s['load_factor'] * float(l_hash)
        # --------------------------------------------------
        # compare
        # --------------------------------------------------
        if l_wtd_hash > l_max_hash:
            l_max_hash = l_wtd_hash
            l_selected_idx = l_idx
        # --------------------------------------------------
        # incr index
        # --------------------------------------------------
        l_idx += 1
    # ------------------------------------------------------
    # increment selected server
    # ------------------------------------------------------
    a_servers[l_selected_idx]['count'] += 1

# ------------------------------------------------------------------------------
# carp
# ------------------------------------------------------------------------------
def carp(a_config_file='',
         a_uris_file=''):

    # ------------------------------------------------------
    # read in config
    # ------------------------------------------------------
    l_config = {}
    with open(a_config_file, 'r+') as l_f:
        l_config = json.load(l_f)
    # ------------------------------------------------------
    # calculate load factors
    # ------------------------------------------------------
    calc_load_factors(l_config)
    # ------------------------------------------------------
    # get servers array
    # ------------------------------------------------------
    l_servers = l_config['servers']
    # ------------------------------------------------------
    # init counts
    # ------------------------------------------------------
    for i_s in l_config['servers']:
        i_s['count'] = 0
    # ------------------------------------------------------
    # read uris line by line
    # ------------------------------------------------------
    l_idx = 0
    with open(a_uris_file) as l_f:
        for i_line in l_f:
            l_uri = i_line.rstrip()
            # ----------------------------------------------
            # carp to server by uri (path)
            # ----------------------------------------------
            server_carp(l_servers, l_uri)
    # ------------------------------------------------------
    # display results
    # ------------------------------------------------------
    print('--------------------------------------------------------------------------------')
    print('{:16} {:24} {:8} {:10} {:16}'.format(
        '    name',
        '            hash',
        '  weight',
        '  load_factor',
        '        count'))
    print('--------------------------------------------------------------------------------')
    for i_s in l_config['servers']:
        print('{:16} {:24d} {:8.2f} {:10.4f} {:16d}'.format(
            i_s['name'],
            i_s['hash'],
            i_s['weight'],
            i_s['load_factor'],
            i_s['count']))

# ------------------------------------------------------------------------------
# main
# ------------------------------------------------------------------------------
def main(argv):

    arg_parser = argparse.ArgumentParser(
                description='carp (carp simulation.)',
                usage= '%(prog)s',
                epilog= '')
    arg_parser.add_argument('-c',
                            '--config',
                            dest='config',
                            help='Back Server Config (json).',
                            required=True)
    arg_parser.add_argument('-u',
                            '--uris',
                            dest='uris',
                            help='URIs file (line delimited).',
                            required=True)
    args = arg_parser.parse_args()
    carp(a_config_file=args.config,
         a_uris_file=args.uris)

# ------------------------------------------------------------------------------
# from cmd line
# ------------------------------------------------------------------------------
if __name__ == "__main__":
    main(sys.argv[1:])
