/**
 * @file mbox.cpp
 */

#include "mbox.h"

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Empty constructor
 *
 * @par Class:
 *    Mbox  
 *
 * @returns n/a
 *
 ******************************************************************************/
Mbox::Mbox()
{
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Delete semaphors and remove shared memory 
 *
 * @par Class:
 *    Mbox
 *
 * @returns n/a
 *
 ******************************************************************************/
Mbox::~Mbox()
{
   //Delete the semaphores if this process created them
   if(creator)
   {
      sem->remove();
   }

   //Delete instances
   delete sem;
   delete shrmem; //Detach memory and flag for deletion
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Initialize mailboxes
 *
 * @par Class:
 *    Mbox
 *
 * @returns n/a
 *
 ******************************************************************************/
void Mbox::init(vector<string> params)
{
   int box_count;    //Number of mailboxes to create
   int size;         //Number of kb for each box
   creator = true;   //Assume creator until proven otherwise

   //Check valid paramters
   if(!(params.size() == 3 && isinteger(params[1]) && isinteger(params[2])) )
   {
      cout << "Usage: mboxinit <number of mailboxes>  <size of mailbox in kbytes>" << endl;
      return;
   }
   
   //Parse params to ints
   box_count = stringToInt(params[1]);
   size = stringToInt(params[2]);

   //Create (or attach) shared memory
   shrmem = new(nothrow)Shrmem(box_count, size*1024);

   //Check if there was a dimension change
   if(shrmem->get_block_size() != size*1024 || shrmem->get_block_count() != box_count)
   {
      //Attaching to an already created memory, inform user
      cout << "Attaching to a mailbox that has already been created." << endl;
      cout << "     Number of Mailboxs: " << shrmem->get_block_count() << endl;
      cout << " Size of Mailboxes (kb): " << (shrmem->get_block_size()/1024) << endl;
      creator = false;
   }

   //Create sempaphores
   sem = new(nothrow)Semaphores(3*shrmem->get_block_count());
   sem->setCols(3);

}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Delete the mailboxes created
 *
 * @par Class:
 *    Mbox
 *
 * @returns n/a
 *
 ******************************************************************************/
void Mbox::del(vector<string> params)
{
   //Check if mailboxes haven't been created
   if(sem == NULL || shrmem == NULL)
   {
      cout << "No mailboxes created. Call mailboxinit first." << endl;
      return;
   }

   //If this process created shared memory, remove semaphores
   if(creator)
   {
      sem->remove();
   }

   //Free memory and set pointers to null
   delete sem;
   delete shrmem;
   sem = NULL;
   shrmem = NULL;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Write to mailbox inputted from the user
 *
 * @par Class:
 *    Mbox
 *
 * @returns n/a
 *
 ******************************************************************************/
void Mbox::write(vector<string> params)
{
   string input = "";         //Text to write to mailbox
   string temp;               //Used for input
   Semaphores & s = (*sem);   //Create a reference pointing to sem
   int box;                   //Index for mailbox to write to

   //Check if mailboes have been created
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

   //Write to memory
   lockandwrite(box, input);

}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Read a mailbox
 *
 * @par Class:
 *    Mbox
 *
 * @returns n/a
 *
 ******************************************************************************/
void Mbox::read(vector<string> params)
{
   int box;    //Index of mailbox to read

   //Check if mailboxes have been created
   if(shrmem == NULL)
   {
      cout << "No mailboxes initialized. Call mboxinit." << endl;
      return;
   }

   //Check for proper usage
   if(!(params.size() == 2 && isinteger(params[1])))
   {
      cout << "Usage: mboxread <boxnumber>" << endl;
      return;
   }

   //Parse parameters to ints
   box = stringToInt(params[1]);
   if(box >= shrmem->get_block_count())
   {
      cout << "No mailbox with index " << box << endl;
      return;
   }

   //Read from memory and output to user
   cout << lockandread(box) << endl;

}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Copy from one mailbox to another
 *
 * @par Class:
 *    Mbox
 *
 * @returns n/a
 *
 ******************************************************************************/
void Mbox::copy(vector<string> params)
{
   int box1, box2;   //Indexes to mailboxes to copy between
   
   //Check if mailboxes have been created
   if(shrmem == NULL)
   {
      cout << "No mailboxes initialized. Call mboxinit." << endl;
      return;
   }

   //Check for proper usage
   if(!(params.size() == 3 && isinteger(params[1])&& isinteger(params[2])))
   {
      cout << "Usage: mboxcopy <source_boxnumber> <dest_boxnumber>" << endl;
      return;
   }

   //Parse paramters to ints
   box1 = stringToInt(params[1]);
   box2 = stringToInt(params[2]);

   //Check for valid indexes
   if(box2 >= shrmem->get_block_count() || box1 >= shrmem->get_block_count())
   {
      cout << "Invalid mailbox index" << endl;
      return;
   }

   //Read from box1, then write to box2
   //Avoids deadlock because both resources are not locked at the same time
   lockandwrite(box2, lockandread(box1));
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Lock mailbox at index box and read from it. 
 *
 * @par Class:
 *    Mbox
 *
 * @param[in] int box - index of mailbox to read from
 * @param[out] string - content read from block
 *
 * @returns n/a
 *
 ******************************************************************************/
string Mbox::lockandread(int box)
{
   Semaphores & s = (*sem);   //Create reference to sem
   Shrmem & shr = (*shrmem);  //Create reference to shr
   string in;                 //Contents of mailbox

   //Install shared variables
   int * reader_count = shr.reader_array();

   //Ensure only 1 process is modifying reader_count array
   s[box][MUTEX].wait();
   reader_count[box]++;
   if(reader_count[box] == 1) //This process is the first reader
   {
      //Release mutex
      s[box][MUTEX].signal();

      //Wait for RW access
      s[box][RW].wait();

      //Realease readers waiting in queue
      s[box][READERS].setval(0);

      //Set back to 1. This prevents readers from continually entering
      //Only currently waiting readers can enter
      s[box][READERS].setval(1);
   }
   else 
   {
      //Release mutex
      s[box][MUTEX].signal();

      //Another reader is waiting for RW access
      //Hop on READERs semaphore and wait for the other process to flag 
      //to continue
      s[box][READERS].waitfor0();
   }

   //Read from the mailbox
   in = shr[box].read();

   //Wait for mutex on reader_count
   s[box][MUTEX].wait();
   reader_count[box]--;
   //Check if last reader (last one out the door)
   //If so, give up RW access
   if(reader_count[box] == 0) 
      s[box][RW].signal();
   //Release mutex
   s[box][MUTEX].signal();

   return in;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Lock a mailbox and write to it.
 *
 * @par Class:
 *    Mbox
 *
 * @param[in] int box - index of mailbox to write to
 * @param[in] string input - data to write to mailbox
 *
 * @returns n/a
 *
 ******************************************************************************/
void Mbox::lockandwrite(int box, string input)
{
   //Create local references
   Semaphores & s = (*sem);
   Shrmem & shr = (*shrmem);

   //Request read/write access to block
   s[box][RW].wait();
   shr[box].write(input.c_str());
   s[box][RW].signal();

   return;
}
