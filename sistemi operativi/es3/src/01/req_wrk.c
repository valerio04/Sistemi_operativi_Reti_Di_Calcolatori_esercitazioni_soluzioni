#include "common.h"

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>


// data array
int * data;
/** COMPLETE THE FOLLOWING CODE BLOCK
*
* Add any needed resource 
**/
//=======
sem_t *sem_worker=NULL, *sem_request=NULL;
int shm_fd;


int request() {
  /** COMPLETE THE FOLLOWING CODE BLOCK
  *
  * map the shared memory in the data array
  **/
 //=================
  
  
  data=(int*) mmap(0,SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd,0);
  if(data==MAP_FAILED){
    perror("mmap");
    exit(1);
  }

 //==============

  printf("request: mapped address: %p\n", data);

  int i;
  for (i = 0; i < NUM; ++i) {
    data[i] = i;
  }
  printf("request: data generated\n");

   /** COMPLETE THE FOLLOWING CODE BLOCK
    *
    * Signal the worker that it can start the elaboration
    * and wait it has terminated
    **/

  //================
  if(sem_post(sem_worker)!=0)
    handle_error("sem_post");
  

  //===============
  if (sem_wait(sem_request)!=0)
    handle_error("sem_wait");
  //=========================
  printf("request: acquire updated data\n");

  printf("request: updated data:\n");
  for (i = 0; i < NUM; ++i) {
    printf("%d\n", data[i]);
  }

   /** COMPLETE THE FOLLOWING CODE BLOCK
    *
    * Release resources
    **/
   //=======
   
   if(munmap(data,SIZE)==-1){
    handle_error("munmap");
   }
   
  ///=========

  return EXIT_SUCCESS;
}

int work() {

  /** COMPLETE THE FOLLOWING CODE BLOCK
  *
  * map the shared memory in the data array
  **/

 
 //=====================
 data=(int*)mmap(0,SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
 if (data==MAP_FAILED){
  perror("mmap");
  exit(1);
 }

 //==================
  printf("worker: mapped address: %p\n", data);

   /** COMPLETE THE FOLLOWING CODE BLOCK
    *
    * Wait that the request() process generated data
    **/

  printf("worker: waiting initial data\n");
  //==============================
    if(sem_wait(sem_worker)!=0)
      handle_error("sem_wait");

  
  //=========
  printf("worker: initial data acquired\n");

  printf("worker: update data\n");
  int i;
  for (i = 0; i < NUM; ++i) {
    data[i] = data[i] * data[i];
  }

  printf("worker: release updated data\n");

   /** COMPLETE THE FOLLOWING CODE BLOCK
    *
    * Signal the requester that elaboration terminated
    **/
   //=========
    if(sem_post(sem_request)!=0)
      handle_error("sem_post");
   //=========


  /** COMPLETE THE FOLLOWING CODE BLOCK
   *
   * Release resources
   **/
  //===========
  
  if(sem_close(sem_worker)==-1)
    handle_error("[@work] sem_worker");
  if(sem_close(sem_request)==-1)
    handle_error("sem_close");
  munmap(data,SIZE);
  if(close(shm_fd)==-1)
      handle_error("close");
  

  //==========

  return EXIT_SUCCESS;
}



int main(int argc, char **argv){
  shm_unlink(SHM_NAME);
  sem_unlink(SEM_NAME_REQ);
  sem_unlink(SEM_NAME_WRK);
   /** COMPLETE THE FOLLOWING CODE BLOCK
    *
    * Create and open the needed resources 
    **/
   //=====================
   shm_fd=shm_open(SHM_NAME, O_CREAT | O_RDWR | O_EXCL, 0600);
   if(shm_fd==-1){
    perror("shm_open");
    exit(1);
   }
   
   if(ftruncate(shm_fd,SIZE)==-1){
    perror("frtuncate");
    exit(1);
   }

   sem_worker= sem_open(SEM_NAME_WRK, O_CREAT | O_EXCL, 0600,0);
   if (sem_worker==SEM_FAILED){
     handle_error("[@main]sem_worker");
   }
   sem_request=sem_open(SEM_NAME_REQ,O_CREAT | O_EXCL, 0600,0);
   if (sem_request==SEM_FAILED){
     handle_error("sem_worker");
   }

   //========================



    int ret;
    pid_t pid = fork();
    if (pid == -1) {
        handle_error("main: fork");
    } else if (pid == 0) {
        work();
        _exit(EXIT_SUCCESS);
    }

    request();
    int status;
    ret = wait(&status);
    if (ret == -1)
      handle_error("main: wait");
    if (WEXITSTATUS(status)) handle_error_en(WEXITSTATUS(status), "request() crashed");


   /** COMPLETE THE FOLLOWING CODE BLOCK
    *
    * Close and release resources
    **/

  if(sem_close(sem_worker)==-1)
    handle_error("[@main] sem_worker");
  if(sem_close(sem_request)==-1)
    handle_error("sem_close");
    if(sem_unlink(SEM_NAME_REQ))
      handle_error("sem_unlink");
    if(sem_unlink(SEM_NAME_WRK))
      handle_error("sem_unlink");
    if(close(shm_fd)==-1)
      handle_error("close");
  
    if(shm_unlink(SHM_NAME))
      handle_error("sem_unlink");

    _exit(EXIT_SUCCESS);

}
