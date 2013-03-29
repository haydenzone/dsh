/**
 * @file mbox.h
 */

#ifndef __MBOX_H_
#define __MBOX_H_

#include "helper.h"
#include<string>
#include<iostream>
#include<vector>
#include "Semaphore.h"
#include "Shrmem.h"

using namespace std;


enum {RW, READERS, MUTEX};

class Mbox {
public:
   Mbox();
   ~Mbox();

   void init(vector<string> params);
   void del(vector<string> params);
   void write(vector<string> params);
   void read(vector<string> params);
   void copy(vector<string> params);
private:
   Semaphores * sem;
   Shrmem * shrmem;
   bool creator;
};
/*
Mailbox initialization
Usage: mboxinit <number of mailboxes>  <size of mailbox in kbytes>
Create k+1 blocks 
k = number of mailboxes requested
block size = size of the mailbox
The first block of shared memory will contain the mailbox information for all to access.
The next k blocks will act as the mailboxes
Mailbox removal
Usage:  mboxdel
Clean up semaphores and shared memory.  This must be done by the creating shell on exit if not before.
Write data
Usage:  mboxwrite <boxnumber>
 ...  data to be written  ...
ctrl-d  to end
Truncate if data is larger than the mailbox and flag user.
Read data
Usage:  mboxread <boxnumber>
Will read iin the text until end of the string or the end of the mailbox is found.
Copy data
Usage: mboxcopy <boxnumber1> <boxnumber2>
Copy the contents of one box to another.
*/
#endif
