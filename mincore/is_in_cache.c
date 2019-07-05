#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>

int main(int argc, char **argv){

        if (argc < 2){
                fprintf(stderr, "Usage: %s FILE\n", argv[0]);
                return 1;
        }
        
        // open a file
        int fd = open(argv[1], O_RDONLY);
        // get file size
        struct stat l_stat = { 0 };
        if(fstat(fd, &l_stat) == -1){
                fprintf(stderr, "Failed to get file size: %s\n", strerror(errno));
                return 2;
        }
        
        // mmap whole file
        void *mapping = NULL;
        if((mapping =
            mmap(NULL, l_stat.st_size, 
                 PROT_READ, MAP_SHARED,
                 fd, 0)) == NULL){
                fprintf(stderr, "Failed to mmap: %s\n", strerror(errno));
                return 3;
        }

        long PAGE_SIZE = sysconf(_SC_PAGESIZE);
        // mincore on the file and printout percentage used
        int alloc_len = (l_stat.st_size+PAGE_SIZE-1) / PAGE_SIZE;
        unsigned char *vec = (unsigned char*)malloc(alloc_len);
        
        if(vec == NULL ||
           mincore(mapping, l_stat.st_size, vec) == -1){
                fprintf(stderr, "Failed to allocate or mincore: %s\n", strerror(errno));
                return 4;
        }
        // got
        
        // print vec
        int i = 0;
        for(; i < alloc_len; ++i)
                fprintf(stdout, "%1x", vec[i]);
        fprintf(stdout, "\n");
        
	return 0;

}
