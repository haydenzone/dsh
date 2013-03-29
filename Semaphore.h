#ifndef __SEMAPHORES_H_
#define __SEMAPHORES_H_
/***************************************************************************//**
 * @file  Semaphore.h
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This class creates an easy to use and intuitive interface for the underlying
 *    semaphore functions.
 *
 * @par Class:
 *    Semaphores
 *
 ******************************************************************************/

/***************************************************************************//**
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string>
#include<iostream>
#include<cerrno>

using namespace std;

/***************************************************************************//**
 * Unions
 ******************************************************************************/
union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};

class Semaphores {
   public:
      /**
       * Class Name: Sem
       * Description:   
       *    This class is a quickly instantiated, then removed, temporary class. It allows
       *    for arrays of semaphores to be access like so:
       *       sems[0].wait() // Wait for the semaphore at index 0
       */
      class Sem 
      {
         public:
            //Data members
            int sem_i;           //Index of semaphore pointing to 
            int row;             //Number of rows if class is 2d
            int cols;            //Number of cols if class is 2d
            Semaphores * parent; //Pointer to parent

            //Methods (essentially point methods to thier parents equivalent)
            void wait() { parent->wait(cols*row+sem_i); };
            void signal() { parent->signal(cols*row+sem_i); };
            void waitfor0() { parent->waitfor0(cols*row+sem_i); };
            void setval(int val) { parent->setval(cols*row+sem_i, val); };
            Semaphores::Sem operator[](int index){ row = sem_i; sem_i = index; return (*this);};
      };

      //These methods allow for child class to access private members in Semaphores class
      friend void Semaphores::Sem::wait();
      friend void Semaphores::Sem::waitfor0();

      //Constructors/destructores
      Semaphores(int count);
      ~Semaphores();

      //Methods
      Semaphores::Sem operator[](int index);
      void setCols(int val);
      void remove();


   private:
      //Methods
      void wait(int sem_i);
      void signal(int sem_i);
      void waitfor0( int sem_i);
      void setval(int sem_i, int val);
      key_t getSemkey();
      int getSemid();   
      int initSem( int val );

      //Data members
      string keypath;   //Pathkey used by ftok
      int keyid;        //Keyid used by ftok
      key_t semkey;     
      int sem_count;    //Number of semaphores
      int semid;        //Semaphore id
      int cols;         //Num of columns if 2-d


};
#endif
