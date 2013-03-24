#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string>
#include<iostream>
#include<cerrno>

using namespace std;

class Semaphores {
public:
   class Sem 
   {
      public:
      int sem_i;
      Semaphores * parent;
      void wait() { parent->wait(sem_i); };
      void signal() { parent->signal(sem_i); };
   };
   friend void Semaphores::Sem::wait();
   Semaphores();
   ~Semaphores();
   Semaphores::Sem operator[](int index);


private:
   void wait(int sem_i);
   void signal(int sem_i);
   string keypath;
   int keyid;
   key_t semkey;
   int sem_count;
   int semid;
   key_t getSemkey();
   int getSemid();
   int initSem();

   
};
