#include<iostream>
#include "Shrmem.h"

using namespace std;

int main(int argc, char ** argv)
{
   Shrmem s(10, 1000);
   int mailbox;
   string input;
   string temp = "";
   do
   {
      cout <<  "> ";
      input = "";
      while(!cin.eof())
      {
         getline(cin, temp);
         if(temp.length() == 1)
         {
            input = temp;
            break;
         }
         else if(temp == "exit")
            return 0;
         else
            input += temp + "\n";
      }
      cin.clear();
      mailbox = input[0] -'0';
      if(input.length() == 0)
         continue;
      if(!(mailbox >= 0 && mailbox < 10))
      {
         cout << "Mailbox must be between 0 and 10" << endl;
         continue;
      }

      if(input.length() == 1)
      {
         cout << "Reading message stored in mailbox " << mailbox << endl;
         cout << "Stored message: " << s[mailbox].read() << endl;
      }
      else if(input.length() > 1)
      {
         input = input.substr(1);
         cout << "Writing message: " << input << " in mailbox " << mailbox << endl;
         s[mailbox].write(input);
      }
      else
      {
         cout << "No args :(" << endl;
      }
   }
   while(input != "exit");
   return 0;
}
