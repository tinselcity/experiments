//: ----------------------------------------------------------------------------
//: Name        : ipc_mutex
//: Author      : Reed Morrison
//: Version     : NA
//: Copyright   : NA
//: Description : Shared mutex example
//: ----------------------------------------------------------------------------

// Includes
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/mman.h>

//: ----------------------------------------------------------------------------
//: Constants...
//: ----------------------------------------------------------------------------
#define MY_COOL_MUTEX_FILE "MY_COOL_MUTEX_FILE"
#define RUN_FOR_ITERATIIONS 20

//: ----------------------------------------------------------------------------
//: Create...
//: ----------------------------------------------------------------------------
int create_mutex(pthread_mutex_t **a_mutex)
{

        // Open file to back mutex object
        int l_mmap_fd = 0;
        l_mmap_fd = open(MY_COOL_MUTEX_FILE, O_RDWR | O_CREAT, 0666);
        if(l_mmap_fd == -1) {

                printf("Error opening file: %s\n", MY_COOL_MUTEX_FILE);
                return -1;

        }

        // Allocate and map into memory
        size_t l_mmap_len = sizeof(pthread_mutex_t);

        int l_fallocate_status = 0;
        l_fallocate_status = posix_fallocate(l_mmap_fd, 0, l_mmap_len);
        if(l_fallocate_status != 0) {

                printf("Error performing fallocate of size: %Zu\n", l_mmap_len);
                close(l_mmap_fd);
                return -1;

        }

        void *l_mmap_ptr = NULL;
        l_mmap_ptr = mmap(NULL, l_mmap_len, PROT_READ | PROT_WRITE, MAP_SHARED, l_mmap_fd, 0);
        if(l_mmap_ptr == ((void *)-1)) {

                printf("Error performing mmap of size: %Zu\n", l_mmap_len);
                close(l_mmap_fd);
                return -1;

        }

        *a_mutex = l_mmap_ptr;

        // Specialize mutex attributes for "shared" mutex
        pthread_mutexattr_t l_mutex_attr;
        pthread_mutexattr_init(&l_mutex_attr);
        pthread_mutexattr_setpshared(&l_mutex_attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(*a_mutex, &l_mutex_attr);
        // TODO Check for errors.

        return 0;

}

//: ----------------------------------------------------------------------------
//: Open...
//: ----------------------------------------------------------------------------
int open_mutex(pthread_mutex_t **a_mutex)
{

        // Open file to back mutex object
        int l_mmap_fd = 0;
        l_mmap_fd = open(MY_COOL_MUTEX_FILE, O_RDWR);
        if(l_mmap_fd == -1) {

                printf("Error opening file: %s\n", MY_COOL_MUTEX_FILE);
                return -1;

        }

        // Allocate and map into memory
        size_t l_mmap_len = sizeof(pthread_mutex_t);

        void *l_mmap_ptr = NULL;
        l_mmap_ptr = mmap(NULL, l_mmap_len, PROT_READ | PROT_WRITE, MAP_SHARED, l_mmap_fd, 0);
        if(l_mmap_ptr == ((void *)-1)) {

                printf("Error performing mmap of size: %Zu\n", l_mmap_len);
                close(l_mmap_fd);
                return -1;

        }

        // Get shared object
        *a_mutex = l_mmap_ptr;

        return 0;

}

//: ----------------------------------------------------------------------------
//: Main...
//: ----------------------------------------------------------------------------
int main(void)
{

        pthread_mutex_t *a_mutex = NULL;
        int l_status = 0;
        int i;


#ifdef WRITER
        printf(": Creating Mutex.\n");
        l_status = create_mutex(&a_mutex);
        if(l_status != 0) {

                printf("Error performing create_mutex.\n");
                return -1;

        }
#else
        printf(": Opening Mutex.\n");
        l_status = open_mutex(&a_mutex);
        if(l_status != 0) {

                printf("Error performing open_mutex.\n");
                return -1;

        }
#endif


        for(i = 0; i < RUN_FOR_ITERATIIONS; ++i) {

#ifdef WRITER
                printf(": Mutex[%3d] -Unlock\n", i);
                pthread_mutex_unlock(a_mutex);
                sleep(2);
#else
                printf(": Mutex[%3d] -Try Lock.\n", i);
                pthread_mutex_lock(a_mutex);
                printf(": Mutex[%3d] -LOCKED\n", i);
#endif


        }

        // TODO Cleanup...

        return 0;

}



