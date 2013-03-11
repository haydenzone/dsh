/**
 * @file Processes.cpp
 */
#include "Processes.h"
/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Empty constructor
 *
 * @par Class:
 *    Processes
 *
 * @returns n/a
 *
 ******************************************************************************/
Processes::Processes() {
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Return name of process with pid
 *
 * @par Class:
 *    Processes
 *
 * @param[in] string pid - pid to be matched
 * @param[in] bool err - flag for if the process doesn't exist
 *
 * @returns n/a
 *
 ******************************************************************************/
string Processes::matchPID( string pid, bool & err )
{
   vector<Process>::iterator it; //iterator
   Process cur;                  //Temp for current process

   //Get the current processes
   getProcesses();
   
   err = true;
   
   //Loop through all current processes
   for( it = cur_processes.begin(); it != cur_processes.end(); it++)
   {
      cur = *it;
      if(cur.pid == pid) //Return match
      {
         err = false;
         return cur.name;
      }

   }
   //Otherwise return an empty string
   return "";
   

}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function returns all processes with the substring name.
 *
 * @par Class:
 *    Processes
 *
 * @param[in] string name - substring to search
 *
 * @returns n/a
 *
 ******************************************************************************/
vector<Process> Processes::pidMatches( string name )
{
   vector<Process> matches;      //All matching processes
   vector<Process>::iterator it; //iterator
   Process cur;                  //Temp holder for current process

   //Get the current processes
   getProcesses();
   
   //Loop through all processes
   for( it = cur_processes.begin(); it != cur_processes.end(); it++)
   {
      cur = *it;
      if(cur.name.find(name) != string::npos)
      {
         matches.push_back(cur); //Add all matches to vector
      }

   }
   return matches;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function returns the name PID of the process with name
 *
 * @par Class:
 *    Processes
 *
 * @param[in] string name - name to match
 * @param[in] bool err - flag if the process was not found
 *
 * @returns n/a
 *
 ******************************************************************************/
string Processes::matchName( string name, bool & err)
{
   vector<Process>::iterator it; //Iterator
   Process cur;                  //Temp holder for current process

   //Get the current processes
   getProcesses();
   
   err = true;

   //Loop through all current processes
   for( it = cur_processes.begin(); it != cur_processes.end(); it++)
   {
      cur = *it;
      //Return pid of match
      if(cur.name == name)
      {
         err = false;
         return cur.pid;
      }

   }
   return "";
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Helper function used to print out all processes 
 *
 * @par Class:
 *    Processes
 *
 * @returns n/a
 *
 ******************************************************************************/
void Processes::listProcesses()
{
   vector<Process>::iterator it;
   Process cur;
   //Get the current processes
   getProcesses();
   
   //Print them
   for( it = cur_processes.begin(); it != cur_processes.end(); it++)
   {
      cur = *it;
      cout << cur.pid << " -> " << cur.name << endl;
   }
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Gets the name of a process from the PID
 *
 * @par Class:
 *    Processes
 *
 * @param[in] string PID
 *
 * @returns string name of process
 *
 ******************************************************************************/
string Processes::getProcessName(string PID)
{
   ifstream fin;     //File input stream
   string file;      //Location of status file
   string input;     //Temp buffer for input

   //Open status file
   file = "/proc/"+PID+"/status";
   fin.open(file.c_str());
   if(!fin) //Check success
   {
      cout << "Failure to open " << file << endl;
      return "";
   }

   getline(fin, input); //Input

   //Remove leading: "Name :"
   input.erase(input.begin(), input.begin()+6);

   return input;

}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function gets all the files in the /proc directory
 *
 * @par Class:
 *    Processes
 *
 * @param[out] & files - stores all files found in /proc
 *
 * @returns n/a
 *
 ******************************************************************************/
void Processes::getProcFiles(vector<string> & files)
{
   DIR * directory;
   struct dirent * durp;
   directory = opendir("/proc/");

   //Check for success
   if( directory == NULL) 
   {
      cout << "Error " << errno << " when trying to open /proc/*" << endl;
   }

   //While we can read in files
   while( (durp = readdir(directory)) != NULL)
   {
      files.push_back(string(durp->d_name));
   }
   closedir(directory);
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function grabs all current processes and places them in 
 *    cur_processes.
 *
 * @par Class:
 *    Processes
 *
 * @returns n/a
 *
 ******************************************************************************/
void Processes::getProcesses() {

   Process temp;           //Temp storage for processes
   vector<string> files;   //List of all files in /proc
   
   //Get files in the proc direcotry
   getProcFiles(files);
   
   //Narrow down to numeric names (processes)
   filterNumeric(files);

   //Clear out previous processes array
   cur_processes.erase(cur_processes.begin(), cur_processes.end());

   //Look up process names and store
   for(vector<string>::iterator it = files.begin(); it != files.end(); it++)
   {
      temp.pid = *it;
      temp.name = getProcessName(*it);
      cur_processes.push_back(temp);
   }

   return;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Simple helper function that removes all none numeric entries in a string
 *    vector.
 *
 * @par Class:
 *    Processes
 *
 * @param[in,out] & strs - vector to filter
 *
 * @returns n/a
 *
 ******************************************************************************/
void Processes::filterNumeric( vector<string> & strs )
{
   vector<string>::iterator iter; //Iterator

   for(iter = strs.begin(); iter != strs.end(); iter++)
   {
      if(!isinteger(*iter))
      {
         iter = strs.erase(iter);
         iter--; //Move back 1 element
      }
   }
}


