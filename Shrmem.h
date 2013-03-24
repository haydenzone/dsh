#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include<iostream>
#include<string>
#include<cstdlib>

using namespace std;

class Shrmem {
   public:
   Shrmem(int block_count_i, int block_size_i);
   ~Shrmem();
   class Block {
      public:
      Shrmem * parent;
      int block_i;
      void write(string message) { parent->write(message, block_i); };
      string read() { return parent->read(block_i); }; 
   };
   friend void Block::write(string message);
   friend string Block::read();

   Shrmem::Block operator[](int index);
   private:
   key_t getShmkey();
   key_t shmkey;
   int getShmadd();
   string keypath;
   int keyid;
   int getShm();
   int memsize;
   int shmid;
   void * shm_address;
   void write(string message, int block);
   string read(int block);
   int block_size;
   int block_count;
};
