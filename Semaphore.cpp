/**
 * @file Semaphore.cpp
 */
#include<iostream>
#include "Semaphore.h"

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Create semaphores, or attach to them if already created
 *
 * @par Class:
 *    Semaphores
 *
 * @param[in] int count - number of semaphores
 *
 * @returns n/a
 *
 ******************************************************************************/
Semaphores::Semaphores(int count) 
{
   //Assume 1-d initially
   cols = 1; 

   //Get a unique semaphore key
   keypath = "/dev/null";
   keyid = 1;
   semkey = getSemkey();
   sem_count = count;
   semid = getSemid();
   initSem(1); 
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Empty destructor (must call remove to delete them)
 *
 * @par Class:
 *    Semaphores
 *
 *
 * @returns n/a
 *
 ******************************************************************************/
Semaphores::~Semaphores()
{
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Remove semaphores
 *
 * @par Class:
 *    Semaphores
 *
 * @returns n/a
 *
 ******************************************************************************/
void Semaphores::remove()
{
   //Send flag to remove semaphores
   int rc = semctl( semid, 1, IPC_RMID );

   //Check for failure
   if (rc==-1)
   {
      cout << "semctl() remove failed" << endl;
   } 
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Generate semkey using ftok
 *
 * @par Class:
 *    Semaphores
 *
 * @returns key_t - semaphore key
 *
 ******************************************************************************/
key_t Semaphores::getSemkey()
{
   key_t semkey = ftok(keypath.c_str(), keyid);
   //Check for failure
   if(semkey == (key_t) -1)
      cout << "Ftok failed" << endl;
   return semkey;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Wait for semaphore at sem_i
 *
 * @par Class:
 *    Semaphores
 *
 * @param[in] int sem_i - semaphore to wait for
 *
 * @returns n/a
 *
 ******************************************************************************/
void Semaphores::wait( int sem_i )
{
   //Wait for semaphore
   sembuf ops[1];
   ops[0].sem_num = sem_i;
   ops[0].sem_op = -1; //Decrement
   ops[0].sem_flg = 0; //Wait for it
   int rv = semop(semid, ops, 1);

   //Check for error
   if(rv == -1)
      cout << strerror(errno) << endl;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Wait for a semaphore to become 0
 *
 * @par Class:
 *    Semaphores
 *
 * @param[in] int sem_i - semaphore to wait for
 *
 * @returns n/a
 *
 ******************************************************************************/
void Semaphores::waitfor0( int sem_i )
{
   //Wait for semaphore to go to 0
   sembuf ops[1];
   ops[0].sem_num = sem_i;
   ops[0].sem_op = 0;   //Wait for it to become 0
   ops[0].sem_flg = 0; //Wait
   int rv = semop(semid, ops, 1);

   //Check for errors
   if(rv == -1)
      cout << strerror(errno) << endl;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Signal semphore at index sem_i
 *
 * @par Class:
 *    Semaphores
 *
 * @param[in] int sem_i - semaphore to signal
 *
 * @returns n/a
 *
 ******************************************************************************/
void Semaphores::signal( int sem_i )
{
   //Signal semaphore
   sembuf ops[1];
   ops[0].sem_num = sem_i;
   ops[0].sem_op = 1;      //Increment by 1
   ops[0].sem_flg = IPC_NOWAIT; //Don't wait 
   int rv =semop(semid, ops, 1);

   //Check for errors
   if(rv == -1)
      cout << strerror(errno) << endl;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This methods allows for indexing of Semaphores class.
 *    semaphores.signal(0) <=> semaphores[0].signal()
 *
 * @par Class:
 *    Semaphores
 *
 * @param[in] int index - index of semaphore being operated on
 *
 * @returns Semaphores::Sem
 *
 ******************************************************************************/
Semaphores::Sem Semaphores::operator[](int index)
{
   //Intialize Sem class, used only as temp
   Semaphores::Sem sem;
   sem.sem_i = index;
   sem.row = 0;
   sem.cols = cols;
   sem.parent = this;
   return sem;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Make structure a 2-d array of semaphores
 *
 * @par Class:
 *    Semaphores
 *
 * @param[in] int val - number of columns
 *
 * @returns n/a
 *
 ******************************************************************************/
void Semaphores::setCols(int val)
{
   cols = val;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Get the semid of semaphores
 *
 * @par Class:
 *    Semaphores
 *
 * @returns int - semid
 *
 ******************************************************************************/
int Semaphores::getSemid()
{
   int semid = semget( semkey, sem_count, 0666 | IPC_CREAT | IPC_EXCL );

   //Check for success
   if(semid == -1)
   {
      //Semaphores may have already been created, try to attach
      semid = semget( semkey, sem_count, 0666 );
      if(semid == -1)
      {
         //Absolutely could not attach
         cout << "Completely failed to get semaphore " << semkey << endl;
         return -1;
      }
   }
   return semid;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Set the value of a semaphore
 *
 * @par Class:
 *    Semaphores
 *
 * @param[in] int sem_i - semaphore to modify
 * @param[in] int val - value to set to
 *
 * @returns n/a
 *
 ******************************************************************************/
void Semaphores::setval(int sem_i, int val)
{
   //Set value
   union semun semopts;    
   semopts.val = val;
   int rc = semctl( semid, sem_i, SETVAL,semopts);

   //Check for success
   if(rc == -1)
   {
      cout << "sem setval failed" << endl;
   }
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Intialize semaphores to a value
 *
 * @par Class:
 *    Semaphores
 *
 * @param[in] int val - value to set to
 *
 * @returns n/a
 *
 ******************************************************************************/
int Semaphores::initSem(int val)
{
   //Allocate array of values
   short * sems = new(nothrow)short[sem_count];
   for(int i = 0; i < sem_count; i++)
      sems[i] = val;
   //Set vals
   int rc = semctl( semid, 1, SETALL, sems);
   delete sems;  //Free memory

   //Check for errors
   if(rc == -1)
   {
      cout << "initSem failed" << endl;
   }

   return rc;
}


