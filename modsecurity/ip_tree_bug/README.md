### Sample code:
```c
int main(void)
{
         char *l_err_msg;
         TreeRoot *l_tree;
         apr_pool_t l_mp;
         modsec_rec l_msr;
         int l_s;
         l_s = create_radix_tree(&l_mp, &l_tree, &l_err_msg);
         TreeNode *tnode = NULL;
         tnode = TreeAddIP("192.168.100.0/24", l_tree->ipv4_tree, IPV4_TREE);
         l_s = tree_contains_ip(&l_mp, l_tree, "192.168.100.0", &l_msr, &l_err_msg);
         return 0;
}
```

### Compile with:
```bash
gcc main.c msc_tree.c -I./ -lasan -g3 -fno-omit-frame-pointer -fsanitize=address -Wall
```
### run:
```bash
./a.out
```
### samples results:
```bash
rmorrison@CPU~/gproj/nlocal/c/ip_tree_bug>gcc main.c msc_tree.c -I./ -lasan -g3 -fno-omit-frame-pointer -fsanitize=address -Wall
rmorrison@CPU~/gproj/nlocal/c/ip_tree_bug>./a.out 
Hello world
create_radix_tree: 0
tnode:             0x604000000010
=================================================================
==7520==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x602000000034 at pc 0x000000404f0b bp 0x7ffe8eed7a90 sp 0x7ffe8eed7a80
READ of size 1 at 0x602000000034 thread T0
    #0 0x404f0a in CPTFindElement /home/rmorrison/gproj/nlocal/c/ip_tree_bug/msc_tree.c:852
    #1 0x4050d0 in CPTIpMatch /home/rmorrison/gproj/nlocal/c/ip_tree_bug/msc_tree.c:886
    #2 0x401229 in tree_contains_ip /home/rmorrison/gproj/nlocal/c/ip_tree_bug/main.c:60
    #3 0x4014ff in main /home/rmorrison/gproj/nlocal/c/ip_tree_bug/main.c:94
    #4 0x7fe73fa4582f in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x2082f)
    #5 0x400e38 in _start (/home/rmorrison/gproj/nlocal/c/ip_tree_bug/a.out+0x400e38)

0x602000000034 is located 0 bytes to the right of 4-byte region [0x602000000030,0x602000000034)
allocated by thread T0 here:
    #0 0x7fe73fecd920 in malloc (/usr/lib/x86_64-linux-gnu/libasan.so.4+0xde920)
    #1 0x401600 in apr_palloc /home/rmorrison/gproj/nlocal/c/ip_tree_bug/msc_tree.c:29
    #2 0x401be3 in CPTCreatePrefix /home/rmorrison/gproj/nlocal/c/ip_tree_bug/msc_tree.c:211
    #3 0x4029bc in CPTAddElement /home/rmorrison/gproj/nlocal/c/ip_tree_bug/msc_tree.c:395
    #4 0x405503 in TreeAddIP /home/rmorrison/gproj/nlocal/c/ip_tree_bug/msc_tree.c:954
    #5 0x401490 in main /home/rmorrison/gproj/nlocal/c/ip_tree_bug/main.c:92
    #6 0x7fe73fa4582f in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x2082f)

SUMMARY: AddressSanitizer: heap-buffer-overflow /home/rmorrison/gproj/nlocal/c/ip_tree_bug/msc_tree.c:852 in CPTFindElement
Shadow bytes around the buggy address:
  0x0c047fff7fb0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c047fff7fc0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c047fff7fd0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c047fff7fe0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c047fff7ff0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x0c047fff8000: fa fa 00 00 fa fa[04]fa fa fa 00 00 fa fa 01 fa
  0x0c047fff8010: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8020: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8030: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8040: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8050: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==7520==ABORTING
```
