#include "Shrmem.h"

Shrmem::Shrmem(int block_count_i , int block_size_i)
{
   keypath = "/dev/null";
   keyid = 1;
   //cout << "Creating" << endl;
   shmkey = getShmkey();

   info.block_size = block_size_i;
   info.block_count = block_count_i;
   memsize = info.block_size*info.block_count+sizeof(block_info);

   //Add a block at the beginning to store block information



   if(getShmadd() == -1)
   {
      //cout << "Could not attach memory" << endl;
      exit(-1);
   }
   //cout << "Shmkey: " << shmkey << endl;

}
void* Shrmem::getAddress()
{
   return (void*)(((char*)shm_address)+sizeof(block_info));
}
Shrmem::~Shrmem()
{
   //cout << "Deleting.." << endl;
   int rc = shmdt(shm_address);
   //cout << "RC = " << rc << endl;
   shmid_ds temp;
   shmid_ds * buf = &temp;
   shmctl(shmid, IPC_RMID, buf);
}
key_t Shrmem::getShmkey()
{
   key_t semkey = ftok(keypath.c_str(), keyid);
   if(semkey == (key_t) -1)
      //cout << "Ftok failed" << endl;
   return semkey;
}

int Shrmem::getShmadd()
{
   shmid = shmget(shmkey, memsize, 0666 | IPC_CREAT | IPC_EXCL);
   if (shmid == -1)
   {
      printf("memory may have already been created Attempting to attach..\n");
      //Attach to memory and get block size information
      shmid = shmget(shmkey, sizeof(block_info), 0666);
      if(shmid == -1)
      {
         //cout << "Unable to attach to shared memory" << endl;
         return shmid;
      }
      shm_address = shmat(shmid, NULL, 0);
      info = (*((block_info *)shm_address));

      //Recalculate memsize
      //cout << "Correcting memory size to " << info.block_count << " boxes of " << info.block_size << " bytes each." << endl;
      memsize = info.block_size*info.block_count+sizeof(block_info);

      //Deleting current attachment
      //cout << "Deleting.." << endl;
      int rc = shmdt(shm_address);
      //cout << "RC = " << rc << endl;

      //Reattach correct size
      shmid = shmget(shmkey, memsize, 0666);


   }



   /* Attach the shared memory segment to the server process.       */
   shm_address = shmat(shmid, NULL, 0);
   (*(block_info *)shm_address) = info;

   //TODO Check if block infos match
   if ( shm_address==NULL )
   {
      printf("main: shmat() failed\n");
      return -1;
   }
}
void Shrmem::write(string message, int block)
{
   //TODO Check if block within range
   void * ptr = (void *)((char *)shm_address + block*info.block_size);
   strcpy((char *)ptr, message.c_str());
}
string Shrmem::read(int block)
{
   //TODO Check if block within range
   void * ptr = (void *)((char *)shm_address + block*info.block_size);
   string temp = (char *)ptr;
   return temp;
}

Shrmem::Block Shrmem::operator[](int index)
{
   Shrmem::Block b;
   b.block_i = index;
   b.parent = this;
   return b;
}
