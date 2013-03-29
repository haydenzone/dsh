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
      sem->remove();
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
   if(creator)
   {
      sem->remove();
   }
   delete sem;
   delete shrmem;
   sem = NULL;
   shrmem = NULL;
}
void Mbox::write(vector<string> params)
{
   string input = "";
   string temp;
   Semaphores & s = (*sem);
   int box;

   if(shrmem == NULL)
   {
      cout << "No mailboxes initialized. Call mboxinit." << endl;
      return;
   }

   //Check correct usage
   if(!(params.size() == 2 && isinteger(params[1])))
   {
      cout << "Usage: mboxwrite <boxnumber>" << endl;
      cout << " ... data to be written ... " << endl;
      cout << "ctrl-d to break" << endl;
      return;
   }

   //Input and end on eof flag
   while(!cin.eof())
   {
      getline(cin, temp);
      input += temp;
      if(!cin.eof())
         input += "\n";
   }
   cin.clear();

   //Check if input length is longer than block_size
   if(input.size() > shrmem->get_block_size())
   {
      cout << "Input is too long. Truncating excess." << endl;
   }

   //Convert input number to an int, and check boundaries
   box = stringToInt(params[1]);
   if(box >= shrmem->get_block_count())
   {
      cout << "No mailbox with index " << box << endl;
   }

   lockandwrite(box, input);

}
void Mbox::read(vector<string> params)
{
   int box;

   if(shrmem == NULL)
   {
      cout << "No mailboxes initialized. Call mboxinit." << endl;
      return;
   }

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
   cout << lockandread(box) << endl;

}
void Mbox::copy(vector<string> params)
{
   int box1, box2;
   if(shrmem == NULL)
   {
      cout << "No mailboxes initialized. Call mboxinit." << endl;
      return;
   }
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
   lockandwrite(box2, lockandread(box1));
}

string Mbox::lockandread(int box)
{
   Semaphores & s = (*sem);
   Shrmem & shr = (*shrmem);
   string in;
   //Install shared variables
   int * reader_count = shr.reader_array();

   s[box][MUTEX].wait();
   reader_count[box]++;
   if(reader_count[box] == 1)
   {
      s[box][MUTEX].signal();
      //cout << "Waiting for RW" << endl;
      s[box][RW].wait();
      //cout << "Signaling other readers" << endl;
      s[box][READERS].setval(0);
      s[box][READERS].setval(1);
   }
   else 
   {
      cout << "Waiting in readers queue" << endl;
      s[box][MUTEX].signal();
      s[box][READERS].waitfor0();
   }

   in = (*shrmem)[box].read();

   //cout << "Waiting for mutex" << endl;
   s[box][MUTEX].wait();
   reader_count[box]--;
   //cout << "reader_count = " << reader_count;
   if(reader_count[box] == 0) //Last reader
      s[box][RW].signal();
   s[box][MUTEX].signal();
   return in;
}


void Mbox::lockandwrite(int box, string input)
{
   Semaphores & s = (*sem);
   Shrmem & shr = (*shrmem);
   //Write
   s[box][RW].wait();
   shr[box].write(input.c_str());
   s[box][RW].signal();

   return;
}
