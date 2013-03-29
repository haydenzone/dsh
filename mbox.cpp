/**
 * @file mbox.cpp
 */

#include "mbox.h"

Mbox::Mbox()
{
}
Mbox::~Mbox()
{
   /*
   if(&sem != NULL)
      delete &sem;
   if(&shrmem != NULL)
      delete &shrmem;
      */
   delete sem;
   delete shrmem;
   if(creator)
   {
      //Delete the semaphores
   }
}

void Mbox::init(vector<string> params)
{
   cout << "Mbox::init" <<endl;
   int box_count;
   int size;
   creator = true;
   if(!(params.size() == 3 && isinteger(params[1]) && isinteger(params[2])) )
   {
      cout << "Usage: mboxinit <number of mailboxes>  <size of mailbox in kbytes>" << endl;
      return;
   }
   box_count = stringToInt(params[1]);
   size = stringToInt(params[2]);

   //Create (or attach) shared memory
   shrmem = new(nothrow)Shrmem(box_count, size*1024);
   if(shrmem->get_block_size() != size*1024 || shrmem->get_block_count() != box_count)
   {
      cout << "Attaching to a mailbox that has already been created." << endl;
      cout << "     Number of Mailboxs: " << shrmem->get_block_count() << endl;
      cout << " Size of Mailboxes (kb): " << (shrmem->get_block_size()/1024) << endl;
      creator = false;
   }

   //Create sempaphores
   sem = new(nothrow)Semaphores(3*shrmem->get_block_count());
   sem->setCols(3);

}
void Mbox::del(vector<string> params)
{
   cout << "Mbox::del" << endl;
}
void Mbox::write(vector<string> params)
{
   cout << "Mbox::write" <<endl;
}
void Mbox::read(vector<string> params)
{
   cout << "Mbox::read" << endl;
}
void Mbox::copy(vector<string> params)
{
   cout << "Mbox::copy" << endl;
}
