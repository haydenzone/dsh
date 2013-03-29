/**
 * @file Shrmem.cpp
 */
#include "Shrmem.h"

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Create (or attach) shared memory of specified dimensions
 *
 * @par Class:
 *    Shrmem  
 *
 * @param[in] int block_count_i - input desired block_count
 * @param[in] int block_size_i - inputted desired block_size
 *
 * @returns n/a
 *
 ******************************************************************************/
Shrmem::Shrmem(int block_count_i , int block_size_i)
{
   keypath = "/dev/null";
   keyid = 1;
   shmkey = getShmkey();

   //Set block info
   info.block_size = block_size_i;
   info.block_count = block_count_i;

   //Add a block at the beginning to store block information
   memsize = info.block_size*info.block_count+sizeof(block_info)+info.block_count*sizeof(int);

   if(getShmadd() == -1)
   {
      exit(-1);
   }
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Simple getter for block_size
 *
 * @par Class:
 *    Shrmem  
 *
 * @returns int - block_size
 *
 ******************************************************************************/
int Shrmem::get_block_size()
{
   return info.block_size;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Simple getter for block_count
 *
 * @par Class:
 *    Shrmem  
 *
 * @returns int - block_count
 *
 ******************************************************************************/
int Shrmem::get_block_count()
{
   return info.block_count;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Simple getter for shm_address (skipping past metadata)
 *
 * @par Class:
 *    Shrmem  
 *
 * @returns void * - address
 *
 ******************************************************************************/
void* Shrmem::getAddress()
{
   return (void*)(((char*)shm_address)+sizeof(block_info)+info.block_count*sizeof(int));
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Simple getter for reader count array (skipping past metadata)
 *
 * @par Class:
 *    Shrmem  
 *
 * @returns int * - address
 *
 ******************************************************************************/
int * Shrmem::reader_array()
{
   return (int *)(((char*)shm_address)+sizeof(block_info));
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Detach memory and flag for removal
 *
 * @par Class:
 *    Shrmem  
 *
 * @returns n/a
 *
 ******************************************************************************/
Shrmem::~Shrmem()
{
   int rc = shmdt(shm_address); //Detach memory
   shmid_ds temp;
   shmid_ds * buf = &temp;
   shmctl(shmid, IPC_RMID, buf); //Flag for removal
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Get a shmkey using ftok
 *
 * @par Class:
 *    Shrmem  
 *
 * @returns key_t - shared memory key
 *
 ******************************************************************************/
key_t Shrmem::getShmkey()
{
   key_t semkey = ftok(keypath.c_str(), keyid);
   if(semkey == (key_t) -1)
      //cout << "Ftok failed" << endl;
   return semkey;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Add shared memory to address space
 *
 * @par Class:
 *    Shrmem  
 *
 * @returns int - shmid
 *
 ******************************************************************************/
int Shrmem::getShmadd()
{
   //Attempt to create share memory
   shmid = shmget(shmkey, memsize, 0666 | IPC_CREAT | IPC_EXCL);
   if (shmid == -1)
   {
      //Memory may have already been created, attempt to attach
      //Attach to memory and get block size information
      shmid = shmget(shmkey, sizeof(block_info), 0666);
      if(shmid == -1)
      {
         return shmid;
      }
      shm_address = shmat(shmid, NULL, 0);
      info = (*((block_info *)shm_address));

      //Recalculate memsize
      memsize = info.block_size*info.block_count+sizeof(block_info)+info.block_count*sizeof(int);

      //Deleting current attachment
      shmdt(shm_address);

      //Reattach correct sized memory block
      shmid = shmget(shmkey, memsize, 0666);
   }

   //Attach to memory and set blocks metadata
   shm_address = shmat(shmid, NULL, 0);
   (*(block_info *)shm_address) = info;

   //Check for success
   if ( shm_address==NULL )
   {
      printf("main: shmat() failed\n");
      return -1;
   }
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Write string to specifed block
 *
 * @par Class:
 *    Shrmem  
 *
 * @param[in] string message - message to write
 * @param[in] int block - block to write to
 *
 * @returns n/a
 *
 ******************************************************************************/
void Shrmem::write(string message, int block)
{
   //Get pointer to start of block
   void * ptr = (void *)((char *)getAddress() + block*info.block_size);

   //Write message (clipping at info.block_size
   strncpy((char *)ptr, message.c_str(), info.block_size);

   //Ensure the message is null terminated
   ((char *)ptr)[info.block_size-1]= '\0';
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Read string from specifed block
 *
 * @par Class:
 *    Shrmem  
 *
 * @param[in] int block - block to read to
 *
 * @returns string - message in block
 *
 ******************************************************************************/
string Shrmem::read(int block)
{
   //Get pointer to start of block
   void * ptr = (void *)((char *)getAddress() + block*info.block_size);

   //Copy message to temporary string
   string temp = (char *)ptr;

   return temp;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *     Allows for indexing into Shrmem instances
 *
 * @par Class:
 *    Shrmem  
 *
 * @param[in] int index - block index
 *
 * @returns Shrmem::Block
 *
 ******************************************************************************/
Shrmem::Block Shrmem::operator[](int index)
{
   Shrmem::Block b;
   b.block_i = index;
   b.parent = this;
   return b;
}
