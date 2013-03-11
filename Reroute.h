#ifndef __REROUTE_H_
#define __REROUTE_H_
/***************************************************************************//**
 * @file Reroute.h
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This class takes care of all opening and closing required for redirecting
 *    output and input. Previous file descriptors are saved and placed on a 
 *    stack. This allows for restoration of output or input. 
 *
 *    Common Usage:
 *       redirect(...)
 *       Do some stuff...
 *       restore(...)
 *
 * @par Class:
 *    Reroute
 *
 ******************************************************************************/

/***************************************************************************//**
 * Includes
 ******************************************************************************/
#include<dirent.h>
#include<vector>
#include<string>
#include<iostream>
#include<fstream>
#include<errno.h>
#include "helper.h"
 #include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include<cstdlib>
 #include <arpa/inet.h>
#include<string.h>

using namespace std;

/**
 * Reroute Enum
 * This enum allows for easier reading of calls to Reroute's methods. 
 */
enum {
   R_IN, /*!< maps to STD_IN */
   R_OUT /*!< maps to STD_OUT */
   };

class Reroute {
public:
   //Methods
   Reroute();
   bool redirect(int file_no, int new_file_no);
   bool redirect(int file_no, int input_pipe[]);
   bool redirect(int file_no, string file_name);
   bool redirect(int file_no, string ip, string port);
   bool redirect_remote(int file_no, string port);
   bool restore(int file_no);
private:
   //Data members
   int file_id_stack[3][10]; //Allow storing for stdin, stdout, and stderr
   int stack_count[3];       //Counts for each stack

   //Methods
   void push_stack(int file_no, int backup);
   int pop_stack(int file_no);
   int createSocket(string ip, string port);
   int listenForSocket(string port);
};

#endif

