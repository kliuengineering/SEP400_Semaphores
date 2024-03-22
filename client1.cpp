// client1.cpp - An exercise with named semaphores and shared memory
//
// 04-Aug-21  M. Watler         Created.
// 21-Mar-24  K. Liu            Modified.
//
#include <errno.h>
#include <iostream>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "client.h"

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;
const int CLIENT_NO=1;
bool is_running=true;


/************************************** UTILITY FUNCTIONS BEGINS *****************************************/

// utility #1 - get shared memory key
void GetShmKey(key_t &SharedMemoryKey)
{
    SharedMemoryKey = ftok(MEMNAME, MEM_KEY);
}

// utility #2 - create/get shared memory id
void GetShmID (int &SharedMemoryID, key_t SharedMemoryKey)
{
    SharedMemoryID = shmget( SharedMemoryKey, sizeof(struct Memory), IPC_CREAT | 0666 );

    if ( SharedMemoryID < 0 )
    {
        cerr << "client #" << CLIENT_NO << " fails to create shared memory.\n";
        exit(EXIT_FAILURE);
    }
}

// utility #3 - attaches a ptr to the shared memory
void AttachPtrToShm(int &SharedMemoryID, struct Memory **SharedMemoryPTR)
{
    *SharedMemoryPTR = (struct Memory *)shmat(SharedMemoryID, NULL, 0);

    if ( *SharedMemoryPTR == (void *)-1 )
    {
        cout << "client " << CLIENT_NO << " shmat() error" << endl;
        exit(EXIT_FAILURE);
    }
}

// utility #4 initialize named semaphore
void InitSemaphore(sem_t **semaphore_id)
{
    *semaphore_id = sem_open(SEMNAME, O_CREAT, SEM_PERMS, 0);

    if (*semaphore_id == SEM_FAILED)
    {
        cerr << "sem_open failed.\n";
        exit(EXIT_FAILURE);
    }
}

// utility #5 initialize shared memory PTR
void Write(struct Memory *ShmPTR, int packet_num, unsigned short source_num, unsigned short dest_num)
{
    ShmPTR->packet_no = packet_num;
    ShmPTR->srcClientNo = source_num;
    ShmPTR->destClientNo = dest_num;

    memset(ShmPTR->message, 0, BUF_LEN);
    sprintf(ShmPTR->message, "This is message %d from client %d\n", packet_num + 1, source_num);
}

/************************************** UTILITY FUNCTIONS ENDS *******************************************/


static void sigHandler(int sig)
{
    switch(sig) 
    {
        case SIGINT:
            is_running=false;
	    break;
    }
}


int main(void) 
{
    key_t          ShmKey;
    int            ShmID;
    struct Memory  *ShmPTR;

    

    //Intercept ctrl-C for controlled shutdown
    struct sigaction action;
    action.sa_handler = sigHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    //key_t ftok(const char *pathname, int proj_id);
    //
    //The ftok() function uses the identity of the file named by the given pathname
    //and the least significant 8 bits of proj_id (which must be nonzero) to
    //generate a key_t type suitable for use with msgget(2), semget(2), or shmget(2).
    //TODO: Generate the key here
    GetShmKey(ShmKey);    // Shared memory key

    //int shmget(key_t key, size_t size, int shmflg);
    //
    //shmget() returns the identifier of the shared memory segment associated with
    //the value of the argument key.
    //struct Memory {
    //    int           packet_no;
    //    unsigned char sourceIP[4];
    //    unsigned char destIP[4];
    //    char          message[BUF_LEN];
    //};
    //TODO: Create or get the shared memory id
    GetShmID(ShmID, ShmKey);    

    //void *shmat(int shmid, const void *shmaddr, int shmflg);
    //
    //shmat() attaches the shared memory segment identified by shmid to the
    //address space of the calling process. The attaching address is specified
    //by shmaddr. If shmaddr is NULL, the system chooses a suitable (unused)
    //page-aligned address to attach the segment.
    //TODO: Attach a pointer to the shared memory
    AttachPtrToShm(ShmID, &ShmPTR);

    //TODO:initialize named semaphore, can be used between processes
    sem_t *sem_id;
    InitSemaphore(&sem_id);
    



// waits for client 3 to signal
sem_wait(sem_id);

    for(int i=0; i<NUM_MESSAGES && is_running; ++i) 
    {
        //TODO: Synchronize processes with semaphores

        sleep(1);

    
    sem_wait(sem_id);
	if(ShmPTR->destClientNo==CLIENT_NO) 
    {
        cout<<"Client "<<CLIENT_NO<<" has received a message from client "<<ShmPTR->srcClientNo<<":"<<endl;
	    cout<<ShmPTR->message<<endl;

	    //Send a message to client 2 or 3
        //void Write(struct Memory *ShmPTR, int packet_num, unsigned short source_num, unsigned short dest_num, char msg[])
	    //ShmPTR->srcClientNo=CLIENT_NO;
	    //ShmPTR->destClientNo=2+i%2;//send a message to client 2 or 3
	    //memset(ShmPTR->message, 0, BUF_LEN);
	    //sprintf(ShmPTR->message, "This is message %d from client %d\n", i+1, CLIENT_NO);

        Write(ShmPTR, i, CLIENT_NO, 2+i%2);
    }
    sem_post(sem_id);
    }


    shmdt((void *)ShmPTR);
    shmctl(ShmID, IPC_RMID, NULL);

    cout<<"client1: DONE"<<endl;

    return 0;
}
