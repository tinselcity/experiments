#!/bin/bash
gcc -I./ echo_server.c get_my_ip.c upnp.c -Wall -lminiupnpc -o upnp