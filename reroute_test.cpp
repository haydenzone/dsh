#include<iostream>
#include<string>
#include "Reroute.h"

using namespace std;

int main() 
{
   Reroute r;
   string filename = "out.out";
   string in_file = "file.in";
   string input;
   string input2;
   int mypipe[2];
   pipe(mypipe);

   cout << "begin " << endl;
   r.redirect(R_OUT, filename);
   r.redirect(R_IN, in_file);
   cin >> input2;
   cout << input2 << " < ---- " << endl;
   r.redirect(R_OUT, mypipe);
   cout << "Hello world from my prog in a pipe.." << input << endl;
   r.restore(R_OUT);
   r.redirect(R_IN, mypipe);
   getline(cin, input);
   cout << input << " with stuff..."  << endl;
   getline(cin, input);
   r.restore(R_IN);
   cout << input << " with stuff..."  << endl;
   r.restore(R_OUT);
   r.restore(R_IN);
   cout << "end" << endl;
   
   return 0;
}
