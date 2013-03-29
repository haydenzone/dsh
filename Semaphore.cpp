#include<iostream>
#include "Semaphore.h"

Semaphores::Semaphores(int count) 
{
   //cout << "Semaphore is being created." << endl;
   keypath = "/dev/null";
   keyid = 1;
   cols = 1; 
   semkey = getSemkey();
   //cout << "Semkey: " << semkey << endl;
   sem_count = count;
   semid = getSemid();
   //cout << "Semid: " << semid << endl;
   int rc = initSem(1); //Change BACK TO 0!!!
   //cout << "rc: " << rc << endl;
}

Semaphores::~Semaphores()
{
   //cout << "Destroying" << endl;
   int rc = semctl( semid, 1, IPC_RMID );
   if (rc==-1)
   {
      //cout << "main: semctl() remove id failed" << endl;
   } 

}

key_t Semaphores::getSemkey()
{
   key_t semkey = ftok(keypath.c_str(), keyid);
   if(semkey == (key_t) -1)
      //cout << "Ftok failed" << endl;
      return semkey;
}
void Semaphores::wait( int sem_i )
{
   //cout << "Wait for sem_i = " <<  sem_i << endl;
   sembuf ops[1];
   ops[0].sem_num = sem_i;
   ops[0].sem_op = -1;
   ops[0].sem_flg = 0; //Wait for it
   int rv = semop(semid, ops, 1);
   //if(rv == -1)
   //cout << strerror(errno) << endl;
   //cout << "Semops return => " << rv << endl;
}
void Semaphores::waitfor0( int sem_i )
{
   //cout << "Wait for sem_i = " <<  sem_i << "to become 0" << endl;
   sembuf ops[1];
   ops[0].sem_num = sem_i;
   ops[0].sem_op = 0;
   ops[0].sem_flg = 0; //Wait for it
   int rv = semop(semid, ops, 1);
   //if(rv == -1)
   //cout << strerror(errno) << endl;
   //cout << "Semops return => " << rv << endl;
}
void Semaphores::signal( int sem_i )
{
   cout << "Signaling for sem_i = " <<  sem_i << endl;
   sembuf ops[1];
   ops[0].sem_num = sem_i;
   ops[0].sem_op = 1;
   ops[0].sem_flg = IPC_NOWAIT; //Wait for it
   cout << "Semops return => " << semop(semid, ops, 1) << endl;
}
Semaphores::Sem Semaphores::operator[](int index)
{
   Semaphores::Sem sem;
   sem.sem_i = index;
   sem.row = 0;
   sem.cols = cols;
   sem.parent = this;
   return sem;
}
void Semaphores::setCols(int val)
{
   cols = val;
}

int Semaphores::getSemid()
{
   int semid = semget( semkey, sem_count, 0666 | IPC_CREAT | IPC_EXCL );

   if(semid == -1)
   {
      //cout << "semget failed to create sem. Getting..." << endl;
      semid = semget( semkey, sem_count, 0666 );
      if(semid == -1)
      {
         //cout << "Completely failed to get semaphore " << semkey << endl;
      }
   }
   return semid;
}
void Semaphores::setval(int sem_i, int val)
{
   union semun semopts;    
   semopts.val = val;
   int rc = semctl( semid, sem_i, SETVAL,semopts);
   if(rc == -1)
   {
      //cout << "sem setval failed" << endl;
   }
}
int Semaphores::initSem(int val)
{
   short * sems = new(nothrow)short[sem_count];
   for(int i = 0; i < sem_count; i++)
      sems[i] = val;
   int rc = semctl( semid, 1, SETALL, sems);
   delete sems;

   if(rc == -1)
   {
      //cout << "initSem failed" << endl;
   }

   return rc;
}


