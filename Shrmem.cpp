#include "Shrmem.h"

Shrmem::Shrmem(int block_count_i , int block_size_i)
{
   keypath = "/dev/null";
   keyid = 1;
   cout << "Creating" << endl;
   shmkey = getShmkey();

   block_size = block_size_i;
   block_count = block_count_i;
   memsize = block_size*block_count;
   
   if(getShmadd() == -1)
   {
      cout << "Could not attach memory" << endl;
      exit(-1);
   }
   cout << "Shmkey: " << shmkey << endl;

}
Shrmem::~Shrmem()
{
   cout << "Deleting.." << endl;
   int rc = shmdt(shm_address);
   cout << "RC = " << rc << endl;
   shmid_ds temp;
   shmid_ds * buf = &temp;
   shmctl(shmid, IPC_RMID, buf);
}
key_t Shrmem::getShmkey()
{
   key_t semkey = ftok(keypath.c_str(), keyid);
   if(semkey == (key_t) -1)
      cout << "Ftok failed" << endl;
   return semkey;
}

int Shrmem::getShmadd()
{
    shmid = shmget(shmkey, memsize, 0666 | IPC_CREAT | IPC_EXCL);
    if (shmid == -1)
      {
        printf("memory may have already been created Attempting to attach..\n");
         shmid = shmget(shmkey, memsize-20, 0666);
        if(shmid == -1)
        {
           cout << "Well that was an error..." << endl;
           return shmid;
        }

      }

    /* Attach the shared memory segment to the server process.       */
    shm_address = shmat(shmid, NULL, 0);
    if ( shm_address==NULL )
   {
        printf("main: shmat() failed\n");
        return -1;
   }
}
void Shrmem::write(string message, int block)
{
   void * ptr = (void *)((char *)shm_address + block*block_size);
   strcpy((char *)ptr, message.c_str());
}
string Shrmem::read(int block)
{
   void * ptr = (void *)((char *)shm_address + block*block_size);
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
