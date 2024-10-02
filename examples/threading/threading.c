#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
	DEBUG_LOG("Into Thread function");
	struct timespec reqtime;
    reqtime.tv_sec = 0;

	struct thread_data *thread_func_args = (struct thread_data*) thread_param;
	thread_func_args->thread_complete_success = true;
	reqtime.tv_nsec = thread_func_args->ms_to_wait_before_obtain *1000000;

	nanosleep(&reqtime,NULL);
	DEBUG_LOG("usleep finished");

	int rc = pthread_mutex_lock(thread_func_args->thread_data_mutex);
	if(rc != 0){
		thread_func_args->thread_complete_success = false;
		DEBUG_LOG("Failed in obtaining lock");
	} else {
		usleep(thread_func_args->ms_to_wait_before_release);
		rc = pthread_mutex_unlock(thread_func_args->thread_data_mutex);
		if(rc != 0){
			thread_func_args->thread_complete_success = false;
			DEBUG_LOG("Failed in releasing lock");
		}

	}

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    bool success = true;
    pthread_t* thread_check;

    // As the spawned thread have their own stack, hence struct should be pointer to pass
    struct thread_data *params = malloc(sizeof *params);
    params->thread_data_mutex = mutex;
    params->thread_complete_success = true;
    params->ms_to_wait_before_obtain = wait_to_obtain_ms;
    params->ms_to_wait_before_release = wait_to_release_ms;

    thread_check = (pthread_t *) malloc(sizeof(pthread_t));
	if ( thread_check == NULL ) {
        printf("Memory allocation failure for thread failed\n");
        success = false;
    } else {
        int rc = pthread_create(thread, NULL, threadfunc, params);
        if ( rc != 0 ) {
            printf("pthread_create failed with error %d creating thread %ld\n",rc,*thread);
            success = false;
        }
        printf("Started thread with id %ld\n", (unsigned long int)*thread);
    }
    
    //Not yet sure, why join is not required in this test.
	//pthread_join(*thread, NULL);

	if(!params->thread_complete_success)
	success = false;

    return success;
}

