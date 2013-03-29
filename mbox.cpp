/**
 * @file mbox.cpp
 */

#include "mbox.h"

Mbox::Mbox()
{
}
Mbox::~Mbox()
{
   delete sem;
   delete shrmem;
   if(creator)
   {
      //Delete the semaphores
   }
}

void Mbox::init(vector<string> params)
{
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
   delete sem;
   delete shrmem;
}
void Mbox::write(vector<string> params)
{
   string input = "";
   string temp;
   int box;

   if(!(params.size() == 2 && isinteger(params[1])))
   {
      cout << "Usage: mboxwrite <boxnumber>" << endl;
      cout << " ... data to be written ... " << endl;
      cout << "ctrl-d to break" << endl;
      return;
   }

   while(!cin.eof())
   {
      getline(cin, temp);
      input += temp;
      if(!cin.eof())
         input += "\n";
   }
   cin.clear();

   if(input.size() > shrmem->get_block_size())
   {
      cout << "Input is too long. Truncating excess." << endl;
   }
   box = stringToInt(params[1]);
   if(box >= shrmem->get_block_count())
   {
      cout << "No mailbox with index " << box << endl;
   }
   (*shrmem)[box].write(input.c_str());

}
void Mbox::read(vector<string> params)
{
   int box;

   if(!(params.size() == 2 && isinteger(params[1])))
   {
      cout << "Usage: mboxread <boxnumber>" << endl;
      return;
   }
   box = stringToInt(params[1]);
   if(box >= shrmem->get_block_count())
   {
      cout << "No mailbox with index " << box << endl;
      return;
   }
   cout << (*shrmem)[box].read() << endl;
}
void Mbox::copy(vector<string> params)
{
   int box1, box2;
   if(!(params.size() == 3 && isinteger(params[1])&& isinteger(params[2])))
   {
      cout << "Usage: mboxcopy <source_boxnumber> <dest_boxnumber>" << endl;
      return;
   }
   box1 = stringToInt(params[1]);
   box2 = stringToInt(params[2]);
   if(box2 >= shrmem->get_block_count() || box1 >= shrmem->get_block_count())
   {
      cout << "Invalid mailbox index" << endl;
      return;
   }
   (*shrmem)[box2].write((*shrmem)[box1].read());
}
