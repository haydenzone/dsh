/**
 * @file Processes.h
 */
#ifndef __PROCESSES_H_
#define __PROCESSES_H_
/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Processes is the class responsible to processing process information
 *    found in the /proc directory and making it available.
 *
 * @par Class:
 *    Processes
 *
 ******************************************************************************/
#include<dirent.h>
#include<vector>
#include<string>
#include<iostream>
#include<fstream>
#include<errno.h>
#include "helper.h"

using namespace std;

//Struct for a single instance of a process
struct Process {
   string pid;
   string name;
};
class Processes {
public:
   //Methods
   Processes();
   void listProcesses();
   string matchPID( string pid, bool & err );
   string matchName( string name, bool & err);
   vector<Process> pidMatches( string name );
private:
   //Data members
   vector<Process> cur_processes; //stores a current list of processes

   //Methods
   void getProcesses();
   void filterNumeric( vector<string> & strs );
   string getProcessName(string PID);
   void getProcFiles(vector<string> & files);
};

#endif

