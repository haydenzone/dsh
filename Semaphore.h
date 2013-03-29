#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string>
#include<iostream>
#include<cerrno>

using namespace std;

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};

class Semaphores {
   public:
      class Sem 
      {
         public:
            int sem_i;
            int row;
            int cols;
            Semaphores * parent;
            void wait() { parent->wait(cols*row+sem_i); };
            void signal() { parent->signal(cols*row+sem_i); };
            void waitfor0() { parent->waitfor0(cols*row+sem_i); };
            void setval(int val) { parent->setval(cols*row+sem_i, val); };
            Semaphores::Sem operator[](int index){ row = sem_i; sem_i = index; return (*this);};
      };
      friend void Semaphores::Sem::wait();
      friend void Semaphores::Sem::waitfor0();
      Semaphores(int count);
      ~Semaphores();
      Semaphores::Sem operator[](int index);
      void setCols(int val);
      void remove();


   private:
      void wait(int sem_i);
      void signal(int sem_i);
      void waitfor0( int sem_i);
      void setval(int sem_i, int val);
      string keypath;
      int keyid;
      key_t semkey;
      int sem_count;
      int semid;
      key_t getSemkey();
      int getSemid();
      int initSem( int val );
      int cols;


};
