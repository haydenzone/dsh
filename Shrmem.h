#ifndef __SHRMEM_H_
#define __SHRMEM_H_
/***************************************************************************//**
 * @file Shrmem.h
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This class creates an easy to use interface for underlying shared memory
 *    functionality.
 *
 * @par Class:
 *    Shrmem
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
#include<iostream>
#include<string>
#include<cstdlib>

using namespace std;

/***************************************************************************//**
 * Structs
 ******************************************************************************/
struct block_info {
   int block_size; //Number of bytes in blocks
   int block_count;//Number of blocks
};

class Shrmem {
public:
   //Constructor/destructor
   Shrmem(int block_count_i, int block_size_i);
   ~Shrmem();
   //Nested class that allows for indexing Shrmem objects
   //I.e.: shrmem[0].write("blah");
   class Block {
      public:
      Shrmem * parent; //Pointer creating Shrmem object
      int block_i;     //Index of block

      //Point methods back to parent class
      void write(string message) { parent->write(message, block_i); };
      string read() { return parent->read(block_i); }; 
   };

   //Give child classes access to private data
   friend void Block::write(string message);
   friend string Block::read();

   //Public Methods
   int get_block_size();
   int get_block_count();
   Shrmem::Block operator[](int index);
   void * getAddress();
   int * reader_array();

private:
   //Private Methods
   key_t getShmkey();
   int getShmadd();
   int getShm();
   void write(string message, int block);
   string read(int block);

   //Private data members
   block_info info;     //Information about blocks
   void * shm_address;  //Pointer to shared memory
   int memsize;         //Total size of memory
   int shmid;           //Shared memory id
   int keyid;           //Key id
   key_t shmkey;        //Shared memory key
   string keypath;      //Keypath (used by ftok)
};
#endif
