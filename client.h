// client.h - Header file for shared memory
//
// 04-Aug-21  M. Watler         Created.
// 21-Mar-24  K. Liu            Modified.
//
#ifndef CLIENT_H
#define CLIENT_H

//TODO: A memory name and a semaphore name along with semaphore permissions
const int BUF_LEN=1024;
const int NUM_MESSAGES=30;





/************************** MODIFICATION SECTION BEGINS*********************/

// variables
#define MAX 4


// MEM name
const char MEMNAME[] = "MemDispatch";
const int MEM_KEY = 65;

// Semaphore name
const char SEMNAME[] = "SemDispatch";
const mode_t SEM_PERMS = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

/************************** MODIFICATION SECTION BEGINS*********************/





struct Memory 
{
    int            packet_no;
    unsigned short srcClientNo;
    unsigned short destClientNo;
    char           message[BUF_LEN];
};

void *recv_func(void *arg);
#endif//CLIENT_H
