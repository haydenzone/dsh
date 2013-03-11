/**
 * @file helper.cpp
 */
#include "helper.h"

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Checks to see if the string passed in is a positive integer
 *
 * @param[in] string number
 *
 * @returns true - is a positive integer
 *
 ******************************************************************************/
bool isinteger( string number )
{
   //Loop through every digit and verify it is a number
   string::iterator iter;
   for(iter = number.begin(); iter != number.end(); iter++)
   {
      if(!isdigit(*iter))
         return false;
   }
   return true;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function converts an string to an integer
 *
 * @param[in] string number
 *
 * @returns n/a
 *
 ******************************************************************************/
int stringToInt( string number)
{
   int num;
   istringstream ( number ) >> num; //Magic
   return num;

} 

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Trims every leading instance of chr in str
 *
 * @param[in,out]    str edited in place
 * @param[in]        chr
 *
 * @returns n/a
 *
 ******************************************************************************/
void trimLeadingCharacter( string & str, char chr )
{
   //Erase all leading instances of chr
   string::iterator iter = str.begin();
   while(iter != str.end() && (*iter) == chr)
   {
      iter++;
   }
   str.erase(str.begin(), iter);
   return;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Remove all empty strings from a vector<string>
 *
 * @param[in] vector<string> strs - edited in place
 *
 * @returns n/a
 *
 ******************************************************************************/
void filterBlanks( vector<string> &strs )
{
   vector<string>::iterator iter;

   //Loop through elements of vector
   for(iter = strs.begin(); iter != strs.end(); iter++)
   {
      //Remove elements that are empty
      if((*iter) == "" || (*iter) == "\t")
      {
         iter = strs.erase(iter);
         iter--; //Move back 1 element
      }
   }
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Splits the inputted str on the char divide
 *    split(" My mommy   loves me lots", ' ');
 *    => ("My", "mommy", "loves", "me", "lots")
 *
 * @param[in] str - to be split
 * @param[in] divide - what to divide on
 *
 * @returns n/a
 *
 ******************************************************************************/
vector<string> split( string str, char divide )
{
   string::iterator it;   //Simple iterator
   vector<string> tokens; //Where elements are placed

   //Loop through every character
   it = str.begin(); 
   while(it != str.end())
   {
      //If we've hit a divide, pull off the beginning to current point
      if((*it) == divide)
      {
         tokens.push_back(string(str.begin(), it)); //Push onto tokens
         it++;
         it = str.erase(str.begin(), it); //Erase back to beginning
      }
      else //Move forward
      {
         it++;
      }
   }
   tokens.push_back(str); //Put on the end
   filterBlanks(tokens); //Remove all blanks
   return tokens;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function parses the parameters for a command and returns:
 *      * whether stdout should be redirected
 *      * whether stdin should be redirected
 *      * Removes these arguments from the parameter list
 *
 * @param[in,out] command_array - the list of parameters
 * @param[in,out] rert_stdin - reroute stdin
 * @param[in,out] rert_stdout - reroute stdout
 * @param[in,out] stdin_file - set to redirect input file if rert_stdin is true
 * @param[in,out] stdout_file - set to redirect out file if rert_stdout is true
 *
 * @returns n/a
 *
 ******************************************************************************/
void parse_redirect( vector<string> & command_array, 
                     bool & rert_stdin, 
                     bool & rert_stdout, 
                     string & stdin_file, 
                     string & stdout_file)
{
   int size;
   rert_stdin = false;
   rert_stdout = false;
   for( int i = 0; i < 2; i++ )
   {
      if( command_array.size() >= 3 ) //Minimum for redirection syntax
      {
         size = command_array.size();
         //Check to see if element at vector_size -2 is a > or <
         if(command_array[size-2] == "<" || command_array[size-2] == ">")
         {
            //Re-route stdin
            if(command_array[size-2] == "<")
            {
               rert_stdin = true;
               stdin_file = command_array[size-1];
               
               //Remove the last 2 elements
               command_array.erase(--command_array.end());
               command_array.erase(--command_array.end());

            }
            else // ">" Re-route std out  
            {
               rert_stdout = true;
               stdout_file = command_array[size-1];
               
               //Remove the last 2 elements
               command_array.erase(--command_array.end());
               command_array.erase(--command_array.end());
            }

         }
         else
            break;
      }
   }
   return;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function parses the parameters for a command and returns:
 *      * whether stdout should be redirected (port)
 *      * whether stdin should be redirected (port, ip)
 *      * Removes these arguments from the parameter list
 *
 * @param[in,out] command_array - the list of parameters
 * @param[in,out] rert_stdin - reroute stdin
 * @param[in,out] rert_stdout - reroute stdout
 * @param[in,out] stdin_ip - reroute stdin ip address
 * @param[in,out] stdin_port - reroute stdin port
 * @param[in,out] stdout_port - reroute stdout port
 *
 * @returns n/a
 *
 ******************************************************************************/
void parse_remote_redirect( vector<string> & command_array,
                            bool & rert_stdin, 
                            bool & rert_stdout, 
                            string & stdin_ip,
                            string & stdin_port, 
                            string & stdout_port)
{
   int size;
   rert_stdin = false;
   rert_stdout = false;
   size = command_array.size();
   if(command_array.size() >= 4) //Minimum for input redirection syntax
   {
      if(command_array[size-3] == "((")
      {
         rert_stdin = true;
         stdin_ip = command_array[size-2];
         stdin_port = command_array[size-1];
         
         //Remove the last 3 elements
         command_array.erase(--command_array.end());
         command_array.erase(--command_array.end());
         command_array.erase(--command_array.end());
         return;
      }

   }

   if( command_array.size() >= 3 ) //Minimum for output redirection syntax
   {
      //Check to see if element at vector_size -2 is a > or <
      if(command_array[size-2] == "))")
      {
         rert_stdout = true;
         stdout_port = command_array[size-1];

         //Remove the last 2 elements
         command_array.erase(--command_array.end());
         command_array.erase(--command_array.end());

      }

   }
   return;
}



