#include<iostream>
#include "Semaphore.h"

using namespace std;

int main(int argc, char ** argv)
{
   Semaphores s;
   
   if(argc == 1)
   {
      cout << "1 arg" << endl;
      s[0].wait();
   }
   else
   {
      cout << "More args" << endl;
      s[0].signal();
   }
   return 0;
}
