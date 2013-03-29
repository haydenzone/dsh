/***************************************************************************//**
 * @file dsh.cpp
 *
 * @mainpage Program #2
 *
 * @section course_section Course Information
 *
 * @authors Hayden Waisanen
 *
 * @date January 30, 2013
 *
 * @par Instructor:
 *         Hayden Waisanen
 *
 * @par Course:
 *         Operating Systems - MWTF - 2:00 pm
 *
 * @par Location:
 *         McLaury - Room 313
 *
 * @section program_section Program Information
 *
 * @details This program implements a simple shell with support for 5 
 *    commands including:
 *    
 *       * Cmdnm - returns command name for PID
 *       * Signal - sends signal to another process
 *       * Systat - prints various information about the system
 *       * Exit - terminates execution of this program.
 *    
 *    The program operates very similar to a normal shell, and can gracefully
 *    handle all types of input.
 *
 *    Program 2 additions:
 *       * add support for all system commands
 *       * Additional sheel intrinsic functions
 *       * Implement redirection
 *       * Remote shell pipes
 *
 * @section compile_section Compiling and Usage
 *
 * @par Compiling Instructions:
 *      make
 *
 * @par Usage:
   @verbatim
   $ dsh
   @endverbatim
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 *
 * @bug  Sending a signal 19 causes the program to enter an infinite loop.
 * @bug  Currently, pipes require spaces to function
 *         ls|cat   => does not work
 *         ls | cat => works
 * @bug  Remote piping commands on server side only allow for using port once
 *       The pipe becomes open again after 10 minutes or so. 
 *
 * @todo None
 *
 * @par Modifications and Development Timeline:
   @verbatim
   Date          Modification
   ------------  ---------------------------------------------------------------
   Jan 16, 2013  Completed processing of command parameters
   Jan 18, 2013  Completed processing of proc for cmdnm and pid commands
   Jan 24, 2013  Completed processing of proc for systat
   Jan 27, 2013  Finalized signal functionality, documentation.
   Feb 15, 2013  Modify documentation
   Feb 17, 2013  Implement exec for external commands
   Feb 18, 2013  Implement all redirection except external pipes
   Feb 19, 2013  Finalize external pipes
   Feb 20, 2013  Finalize commenting doxygen and submission
   @endverbatim
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include<errno.h>
#include<fstream>
#include<iostream>
 #include <fcntl.h>
#include<iomanip>
#include<sys/types.h>
#include<stdio.h>
#include<signal.h>
#include<string>
#include<unistd.h>
#include<vector>
#include "Processes.h"
#include "helper.h"
#include<sys/wait.h>
#include <cstdlib>
#include<string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "Reroute.h"
#include "mbox.h"
using namespace std;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
void call_function( string command );
void pid(vector<string> params);
void cmdnm(vector<string> params);
void systat(vector<string> params);
void send_signal(vector<string> params);
void signalCatcher(int n);
void exec_command(vector<string> params);
void changeDir(vector<string> params);

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    The main function handles registering the signal call back functions, 
 *    as well as enters the program into the main event loop. When the user
 *    enters 'exit' the program will terminate.
 *
 * @params none
 *
 * @returns int
 *
 ******************************************************************************/
int main()
{
   string input;
   //Register Signal Listeners
   for(int i = 0; i < 32; i++)
      signal(i, signalCatcher);

   //Main loop for displaying terminal, and waiting for user entry
   do
   {
      cout << "dsh> ";

      getline(cin, input);
      trimLeadingCharacter(input, ' ');
      call_function(input);
      
   }
   while(input != "exit"); //Exit when user enters "exit"

   return 0;
}
/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This is the callback function for when the running process receives a 
 *    signal. It simply prints out the id of the signal received.
 *
 * @param[in] n - id of the signal received
 *
 * @returns n/a
 *
 ******************************************************************************/
void signalCatcher(int n)
{
   cout << "Signal received: " << n << endl;

}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function is responsible for taking in the parameters for the "signal"
 *    command and sending out a signal to the specified pid. Error checking is 
 *    done prior to calling the kill command to verify that the process does, in
 *    fact, exist.
 *
 * @param[in] params - vector of all params used to initiate this command
 *
 * @returns n/a
 *
 ******************************************************************************/
void send_signal(vector<string> params)
{
   bool err;         //Error flag used by the Process method calls
   Processes ps;     //Handles process of /proc/*
   int signal_num;   //Int conversion of signal_num param
   int pid;          //Int conversion of pid param

   //Check if signal was called with the correct num of params
   if(params.size() != 3 )
   {
      cout << "Usage: signal <signal_num> <pid>" << endl;
      return;
   }

   //Determine if signal_num and pid were positive integers
   if(!(isinteger(params[1]) && isinteger(params[2])))
   {
      cout << "<signal_num> and <pid> must be positive integers. " << endl;
      return;

   }

   //Convert parameters to ints
   signal_num = stringToInt(params[1]);
   pid = stringToInt(params[2]);

   //Check if valid signal number is given
   if(signal_num > 64 || signal_num < 0)
   {
      cout << "Invalid Signal Number (1 - 64)" << endl;
      return;
   }


   //Check if PID exists
   ps.matchPID(params[2], err);

   if(!err)
   {
      //If the PID exists, send the signal
      if(kill(pid, signal_num) == -1)
      {
         cout << "Error occured while sending signal";
         cout << signal_num << " to process " << pid << endl;
      }
   }
   else //Otherwise, notify the user
   {
      cout << "No process exists with pid "<< params[2] << endl;
   }

   return;
 

}
/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function allows for dsh to take any command suppoerted by the system
 *    then return the output to stdout. It does this by fork/execing the new 
 *    command (printing out the child pid while doing so)
 *    
 *
 * @param[in] params - vector of all params used to initiate this command
 *
 * @returns n/a
 *
 ******************************************************************************/
void exec_command(vector<string> params)
{
   vector<string>::iterator it; //Used for vector<Process> loop
   char *args[100];
   int i; 
   int child_pid;
   int wait_return;
   int status;
   int num_args;
   rusage r;
   rusage temp;
   char str[50];
   static rusage prev = {0};

   //Convert String vector into char * array
   i = 0;
   for(it = params.begin(); it != params.end(); it++)
   {

      args[i] = (char*)(*it).c_str();
      i++;
   }
   args[i] = NULL;

   

   child_pid = fork();
   if(child_pid == 0)
   {
      execvp(args[0], args);
      perror("dsh does not recognize command.");
      exit(5);
   }
   cerr << "Child process ID: " << child_pid << endl;
   
   wait_return = wait(&status);
   
   //Print the process stats
   cerr << "=========================================================================" << endl;
   cerr << " Process Statistics" <<  endl;
   cerr << "=========================================================================" << endl;
   cerr << "   Shell process " << wait_return << " exited with status " << (status >> 8) << endl << endl;
   getrusage(RUSAGE_CHILDREN, &r);

   //Subtract out all prev
   temp = r;
   r.ru_utime.tv_sec -= prev.ru_utime.tv_sec;
   r.ru_stime.tv_sec -= prev.ru_stime.tv_sec;
   r.ru_utime.tv_usec -= prev.ru_utime.tv_usec;
   r.ru_stime.tv_usec -= prev.ru_stime.tv_usec;
   r.ru_majflt -= prev.ru_majflt;
   r.ru_nswap -= prev.ru_nswap;

   cerr << "   User CPU Time Used  : ";
   sprintf(str, "%ld.%06ld\n", r.ru_utime.tv_sec, r.ru_utime.tv_usec);
   cerr << str;
   cerr << "   System CPU Time Used: ";
   sprintf(str,"%ld.%06ld\n", r.ru_stime.tv_sec, r.ru_stime.tv_usec);
   cerr << str;
   cerr << "   Page faults         : " << r.ru_majflt << endl;
   cerr << "   Page swaps          : " << r.ru_nswap << endl;
   cerr << "=========================================================================" << endl;

   //Store the r for later
   prev = temp;

   return;
   
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function simply imports system information from the /proc directory
 *    and displays it to the user.
 *
 * @param[in] params - vector of all params used to initiate this command
 *
 * @returns n/a
 *
 ******************************************************************************/
void systat(vector<string> params)
{
   string input;        //Temporary buffer for input from files
   ifstream meminfo;    //meminfo file stream
   ifstream version;    //version file stream
   ifstream cpuinfo;    //cpuinfo file stream
   ifstream uptime;     //uptime file stream

   //Check for correct number of parameters
   if(params.size() > 1) 
      cout << "No extra parameters required" << endl;

   //Open "files" with sys info
   meminfo.open("/proc/meminfo");
   version.open("/proc/version");
   cpuinfo.open("/proc/cpuinfo");
   uptime.open("/proc/uptime");

   //Check if all files are open
   if(!(meminfo && version && cpuinfo && uptime))
   {
      cout << "Unable to open files in /prov/*";
      return;
   }

   //Print current process ID
   cout << "DSH Current PID: " << getpid() << endl;

   //Print the linux version information
   getline(version, input);
   cout << "Linux Version: " << input << endl;
   
   //Print the Uptime
   uptime >> input;
   cout << "Uptime (seconds): " <<  input << endl;

   //Print the Memory Usage
   for( int i = 0; i < 2; i++)
   {
      meminfo >> input; //Header
      cout << input << "          ";
      meminfo >> input; //Data
      cout << input << " ";
      meminfo >> input; //Data units
      cout << input << endl;
   }

   //Print CPU information;
   getline(cpuinfo, input); //Ignore the first line
   for(int i = 0; i < 8; i++)
   {
      getline(cpuinfo, input);
      cout << input << endl;
   }


   //Close all the files
   meminfo.close();
   version.close();
   cpuinfo.close();
   uptime.close();
   

}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function routes all commands to their specific functions. It also
 *    handles directs including.
 *    * Remote redirects: (( <ip> <port> OR )) <port>
 *    * File redirects: > file.out OR < file.in
 *    * Pipes: cmd1 | cmd2
 *
 * @param[in] params - vector of all params used to initiate this command
 *
 * @returns n/a
 *
 ******************************************************************************/
void call_function( string command ) 
{
   //Declare static Mbox class to utilize functions
   static Mbox mbox;
   //Separate user command on spaces
   vector<string> command_array = split(command, ' ');

   //Socket Redirection
   bool rert_remote_stdin;    //redirect stdin to socket
   bool rert_remote_stdout;   //redirect stdout to socket
   string stdin_ip;           //Input ip address
   string stdin_port;         //Input port 
   string stdout_port;        //Output port

   //File redirection
   bool rert_stdin = false;   //redirect stdin to file
   bool rert_stdout = false;  //redirect stdout to file
   string stdin_file;         //File to redirect output to
   string stdout_file;        //File to redirect input to

   //Pipes
   int pipe_count = 0;        //Number of pipes created
   int pipes[100][2];         //The pipes themselves
   int command_i;             //Index of command currently being executed

   vector<string> temp;                //Temprary variable
   vector<string> cur_command;         //Temporary variable
   vector< vector<string> > commands;  //Parsed commands seperated on pipes (|)

   //Rerouting class which handles redirection and restoration of output/input
   Reroute r;                       
 
   //Check case when nothing is entered
   if(command_array.size() == 0) 
      return;

   //Re-route stdin or std out if necessary
   //Check for socket redirection
   parse_remote_redirect(command_array, rert_remote_stdin, rert_remote_stdout, stdin_ip, stdin_port, stdout_port);

   //If there is not socket redirection, check for file redirection
   if(!(rert_remote_stdout || rert_remote_stdin))
   {
      parse_redirect( command_array, rert_stdin, rert_stdout, stdin_file, stdout_file);
   }

   //Reroute stdout and stdin to files if necessary
   if(rert_stdin)
   {
      r.redirect(R_IN, stdin_file);
   }
   if(rert_stdout)
   {
      r.redirect(R_OUT, stdout_file);
   }
   if(rert_remote_stdin)
   {
      if(!r.redirect(R_IN, stdin_ip, stdin_port))
      {
         cout << "Unable to open socket. " << endl;
         return;
      }
   }
   if(rert_remote_stdout)
   {
      if(!r.redirect_remote(R_OUT, stdout_port))
      {
         cout << "Unable to open socket." << endl;
         return;
      }

   }
   

   //Count the number of pipes, separate commands on pipes
   for(vector<string>::iterator i = command_array.begin(); i != command_array.end(); i++)
   {
      //Current element is a pipe
      if((*i) == "|")
      {
         //Take current command and push on to commands vector
         pipe_count++;
         commands.push_back(temp);
         //Erase temp to begin next command
         temp.clear();
      }
      else //not a pipe
      {
         //Push element on current command
         temp.push_back(*i);
      }
   }
   //Push last command onto vector
   commands.push_back(temp);
   
   //Create all pipes required
   for(int i = 0; i < pipe_count; i++)
      pipe(pipes[i]);


   //Loop through every command and run them (handling redirection)
   command_i = 0; //Intializer index counter
   for(vector< vector<string> >::iterator i = commands.begin(); i != commands.end(); i++)
   {
      //If we're not at the beginning, close input and use pipe
      if(i != commands.begin())
      {
         //Pop off previous pipe on redirection stack
         if(command_i >= 2)
            r.restore(R_IN);

         //Input into last pipe
         r.redirect(R_IN, pipes[command_i-1]);
         close(pipes[command_i-1][0]);
      }

      //If we're not on the last command redirect output
      if((i+1) != commands.end()) 
      {
         //Pop off previous redirection and restore
         if(command_i >= 1)
            r.restore(R_OUT);

         //Output into current pipe
         r.redirect(R_OUT, pipes[command_i]);
         close(pipes[command_i][1]);
      }
      else //At the end, restore out
      {
         //Handle case where there are not redirects
         if(pipe_count > 0)
            r.restore(R_OUT);
      }
      



      cur_command = (*i);
      //Route cur_commands to correct functions
      if(cur_command[0] == "cmdnm")
         cmdnm(cur_command);
      else if(cur_command[0] == "pid")
         pid(cur_command);
      else if(cur_command[0] == "systat")
         systat(cur_command);
      else if(cur_command[0] == "signal")
         send_signal(cur_command);
      else if(cur_command[0] == "mboxinit")
         mbox.init(cur_command);
      else if(cur_command[0] == "mboxdel")
         mbox.del(cur_command);
      else if(cur_command[0] == "mboxwrite")
         mbox.write(cur_command);
      else if(cur_command[0] == "mboxread")
         mbox.read(cur_command);
      else if(cur_command[0] == "mboxcopy")
         mbox.copy(cur_command);
      else if(cur_command[0] == "exit") {}
      else if(cur_command[0] == "cd")
         changeDir(cur_command);
      else //Pass cur_command to system
         exec_command(cur_command);

      //Increment to next command
      command_i++;
   }

   //Restore input
   if(pipe_count > 0)
   {
      r.restore(R_IN);
   }
   

   //Re-route if necessary
   if(rert_stdin)
   {
      r.restore(R_IN);
   }
   if(rert_stdout)
   {
      r.restore(R_OUT);
   }
   if(rert_remote_stdin)
   {
      r.restore(R_IN);
   }
   if(rert_remote_stdout)
   {
      r.restore(R_OUT);
   }
   

   return;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function allows the user to change the current working directory
 *
 * @param[in] params - vector of all params used to initiate this command
 *
 * @returns n/a
 *
 ******************************************************************************/
void changeDir(vector<string> params)
{
   //Check for the correct number of params
   if(params.size() != 2 )
   {
      cout << "Usage: " << endl;
      cout << "cd <absolute path>" << endl;
      cout << "cd <relative path>" << endl;
      return;
   }

   //Change to directory
   if(chdir(params[1].c_str()) != 0)
      cout << "Unable to change directory" << endl;
   return;
}
/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function finds the process name using the pid
 *
 * @param[in] params - vector of all params used to initiate this command
 *
 * @returns n/a
 *
 ******************************************************************************/
void cmdnm(vector<string> params)
{
   Processes ps;  //Imports pid information from /proc directory
   bool err;      //Flag used by Processes methods
   string name;   //Name of matched PID
   
   //Check for the correct number of parameters
   if(params.size() != 2)
   {
      cout << "Usage: cmdnm <pid>" << endl;
      return;
   }

   //Check if the parameter is an integer
   if(!isinteger(params[1]))
   {
      cout << "<pid> must be a positive integer" << endl;
      return;
   }
   //Match the PID
   name = ps.matchPID(params[1], err);

   //Let the user know if a process wasn't found
   if(!err)
      cout <<  name;
   else 
      cout << "No process with pid "+params[1];
   cout << endl;

   return;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function finds all processes with the given substring in their 
 *    name.
 *
 * @param[in] params - vector of all params used to initiate this command
 *
 * @returns n/a
 *
 ******************************************************************************/
void pid(vector<string> params)
{
   Processes ps;                 //Handles processing of /proc/*
   vector<Process> matches;      //Matching processes
   vector<Process>::iterator it; //Used for vector<Process> loop
   Process cur;                  //Temp storage for process
   
   //Check for the correct number of parameters
   if(params.size() != 2)
   {
      cout << "Usage: pid <process_name>" << endl;
      return;
   }

   //Find all matches
   matches = ps.pidMatches(params[1]);
   if(matches.size() > 0)
   {
      //Print the headers
      cout << setw(15) << left <<  "Process Name" << " " << "PID" << endl;

      //Print all the matches
      for( it = matches.begin(); it != matches.end(); it++)
      {
         cur = *it;
         cout << setw(15) << left <<  cur.name << " " << cur.pid << endl;
      }
   }
   else //If none are found
      cout << "No matching process" << endl;

   return;
}

