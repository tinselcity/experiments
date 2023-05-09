#!/usr/bin/env python
from bcc import BPF
from time import sleep

program = """
BPF_HASH(callers, u64, unsigned long);

TRACEPOINT_PROBE(kmem, kmalloc) {
    u64 ip = args->call_site;
    unsigned long *count;
    unsigned long c = 1;
    count = callers.lookup((u64 *)&ip);
    if (count != 0) {
        c += *count;
    }
    callers.update(&ip, &c);
    return 0;
}
"""

b = BPF(text=program, debug=2)

while True:
    try:
        sleep(1)
        calls = {}
        #print(dir(b))
        for k,v in b["callers"].items():
            calls[(b.ksym(k.value)).decode('utf-8')] = int(v.value)
        sc = {k: v for k, v in sorted(calls.items(), key=lambda item: item[1], reverse=True)}
        for k, v in sc.items():
            print ("{:32s} {}".format(k, v))
        print('\n')
    except KeyboardInterrupt:
        exit()
