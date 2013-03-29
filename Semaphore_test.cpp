#include<iostream>
#include "Semaphore.h"
#include "Shrmem.h"
#include<string>

using namespace std;
enum {RW, READERS, MUTEX};
int main(int argc, char ** argv)
{
   Semaphores s(6);
   s.setCols(3);
   Shrmem shr(1,sizeof(int)+sizeof(bool));
   string input;
   int & reader_count = (*((int *)shr.getAddress()));
   bool & readers_q_open = (*((bool *)((char *)shr.getAddress() +sizeof(int))));
   reader_count = 0;
   readers_q_open = true;
   s[MUTEX].setval(1);
   s[READERS].setval(1);
   s[RW].setval(1);

   /*
      if(argc == 1)
      {
      cout << "Setting sem to 0" << endl;
      s[0].setval(0);
      }
      else
      {
      cout << "More args" << endl;
      s[0].waitfor0();
      }*/

   do 
   {
      cout << "R or W? ";
      cin >> input;
      if(input == "=")
      {
         int col;
         int row;
         int val;
         cout << "row col val: ";
         cin >> row >> col >> val;
         s[row][col].setval(val);
      }
      else if(input == "r")
      {
         cout << "Acquiring lock" << endl;
         s[MUTEX].wait();
         reader_count++;
         if(reader_count == 1)
         {
            s[MUTEX].signal();
            cout << "Waiting for RW" << endl;
            s[RW].wait();
            readers_q_open = false;
            cout << "Signaling other readers" << endl;
            s[READERS].setval(0);
            s[READERS].setval(1);
            readers_q_open = true;
         }
         else 
         {
            cout << "Waiting in readers queue" << endl;
            s[MUTEX].signal();
            s[READERS].waitfor0();
         }

         cout << "Reading...click enter when complete.";
         string in;
         cin >> in;

         s[MUTEX].wait();
         reader_count--;
         cout << "reader_count = " << reader_count;
         if(reader_count == 0) //Last reader
            s[RW].signal();
         s[MUTEX].signal();



      }
      else if(input == "w")
      {
         cout << "Acquiring lock" << endl;
         s[RW].wait();
         cout << "Writing...click enter when complete";
         string in;
         cin >> in;
         cout << "Finishing..." <<endl;
         s[RW].signal();
      }
      else if(input == "c")
      {
         cout << "Reader_count = " << reader_count << endl;
         cout << "reader_q_open = " ;
         if(readers_q_open)
            cout << "true" << endl;
         else 
            cout << "false" << endl;

      }
      else if(input == "s")
      {
         string in;
         cout << "Value to set reader_count? "; 
         cin >> reader_count;
         cout << "Read_q set? T or F";
         cin >> in;
         if(in == "T")
            readers_q_open = true;
         else 
            readers_q_open = false;
      }
      else
         cout << "invalid input" << endl;
   } while(input != "exit");
   return 0;
}
