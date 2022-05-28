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

# ------------------------------------------------------------------------------
# carp
# ------------------------------------------------------------------------------
def calc_load_factors(a_config):

    l_ixs = []
    # ------------------------------------------------------
    # create dict of:
    #   index
    #   weight
    # ------------------------------------------------------
    i_s_idx = 0
    l_w_sum = 0.0
    l_servers = a_config['servers']
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
    print('{}'.format(json.dumps(l_p, indent=4)))
    # ------------------------------------------------------
    # base case
    # ------------------------------------------------------
    # TODO
    # ------------------------------------------------------
    # calculate
    # ------------------------------------------------------
    for i_idx in range(1,l_num_servers):
        print('i_idx: {}'.format(i_idx))
    # TODO
    # ------------------------------------------------------
    # done...
    # ------------------------------------------------------
    return

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
    # read uris line by line
    # ------------------------------------------------------
    l_idx = 0
    with open(a_uris_file) as l_f:
        for i_line in l_f:
            l_uri = i_line.rstrip()
            l_hash = hash(l_uri)
            print('uri: {}: hash({}): {}'.format(l_uri, type(l_hash), l_hash))
            l_idx += 1
            # ----------------------------------------------
            # TODO remove
            # ----------------------------------------------
            if l_idx > 10:
                return

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
