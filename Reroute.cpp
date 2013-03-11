/**
 * @file Reroute.cpp
 */
#include "Reroute.h"
/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Empty constructor
 *
 * @par Class:
 *    Reroute
 *
 * @returns n/a
 *
 ******************************************************************************/
Reroute::Reroute() {
   //Intialize stack counts to 0
   for(int i = 0; i < 3; i++)
      stack_count[i] = 0;
   return;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This temporarily redirects the entered file_no to new_file_no. Must 
 *    eventually be followed by a restore call to restore previous input
 *    or output file descriptor.
 *
 * @par Class:
 *    Reroute
 *
 * @param[in] int file_no - file descriptor to redirect
 * @param[in] int new_file_no - file descriptor to direct to
 *
 * @returns bool - success or failure
 *
 ******************************************************************************/
bool Reroute::redirect(int file_no, int new_file_no)
{
   //Save what is currently in file_no
   push_stack(file_no,dup(file_no));
   close(file_no);
   dup(new_file_no);
   return true;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function temporarily redirect STD_IN or STD_OUT to a pipe. The 
 *    function intelligently chooses which end of the pipe to use
 *
 * @par Class:
 *    Reroute
 *
 * @param[in] int file_no - Redirect STDIN, 0, or STDOUT, 1
 * @param[in] int input_pipe[] - pipe to redirect
 *
 * @returns bool - success or failure
 *
 ******************************************************************************/
bool Reroute::redirect(int file_no, int input_pipe[])
{
   //Pass pipe to the redirect function
   return redirect(file_no, input_pipe[file_no]);
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function temporarily redirect STD_IN or STD_OUT to a file, specified
 *    in the file_name parameter. Must be followed by a restore method call to
 *    restore output or input.
 *
 * @par Class:
 *    Reroute
 *
 * @param[in] int file_no - Redirect STDIN, 0, or STDOUT, 1
 * @param[in] int input_pipe[] - pipe to redirect
 *
 * @returns bool - success or failure
 *
 ******************************************************************************/
bool Reroute::redirect(int file_no, string file_name)
{
   int fpt1; //Temporary storage for input file descriptor
   int fpt2; //Temporary storage for output file descriptor
   
   //Save what is currently in file_no
   push_stack(file_no, dup(file_no));

   if(file_no == 0) //Stdin
   {
      //Attempt to open the file for input
      if ((fpt1 = open(file_name.c_str(), O_RDONLY)) == -1)
      {
         cout << "Unable to open " << file_name <<  " for reading." << endl;
         return false;
      }
      close(file_no);
      dup(fpt1);      // redirect the child input
      close(fpt1);    // close unnecessary file descriptor
   }
   else if(file_no == 1) //Stdout
   {
      //Attemp to open the file for output
      if ((fpt2 = creat(file_name.c_str(), 0644)) == -1)
      {
         cout << "Unable to open " << file_name << " for writing." << endl;
         return false;
      }
      close(file_no);
      dup(fpt2);      // redirect the child output 
      close(fpt2);    // close unnecessary file descriptor
   }
   else //Invalid usage
   {
      cout << "Redirect to file is not supported by anything by STDIN or STDOUT" << endl;
      return false;
   }

   //Everything went fine
   return true;

}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Pops previous file descriptor off stock and restores it to the specified
 *    input or output.
 *
 * @par Class:
 *    Reroute
 *
 * @param[in] int file_no - Redirect STDIN, 0, or STDOUT, 1
 *
 * @returns bool - success or failure
 *
 ******************************************************************************/
bool Reroute::restore(int file_no)
{
   //Pop previous output file descriptor off the stack
   int restore_file_no = pop_stack(file_no);

   close(file_no); // Cloe the current file descriptor
   dup2(restore_file_no, file_no); //Restore descriptor
   close(restore_file_no); //Close useless decriptor
   return true;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Implements a simple stack used for storing previous file descriptors. 
 *    The stacks size is limited to 10, but current program requirements
 *    should not require anymore than that.
 *
 * @par Class:
 *    Reroute
 *
 * @param[in] int file_no - Redirect STDIN, 0, or STDOUT, 1
 * @param[in] int backup - Value to push on to stack
 *
 * @returns n/a
 *
 ******************************************************************************/
void Reroute::push_stack(int file_no, int backup)
{
   //Check if the stack is full
   if(stack_count[file_no] == 10)
   {
      cout << "Reroute stack is full. Exiting..." << endl;
      exit(-1);
   }

   //Place value on stack and increment the stack pointer
   file_id_stack[file_no][stack_count[file_no]++] = backup;
}
/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    Implements a simple stack used for storing previous file descriptors. 
 *    The stacks size is limited to 10, but current program requirements
 *    should not require anymore than that.
 *
 * @par Class:
 *    Reroute
 *
 * @param[in] int file_no - Redirect STDIN, 0, or STDOUT, 1
 *
 * @returns value at top of stack
 *
 ******************************************************************************/
int Reroute::pop_stack(int file_no)
{
   //Check if the stack is empty
   if(stack_count[file_no] == 0)
   {
      cout << "Reroute stack is empty. Exiting...." << endl;
      exit(-1);
   }

   //Return value on stack and decrement stack pointer
   return file_id_stack[file_no][--stack_count[file_no]];
}
/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function temporarily redirect STD_IN or STD_OUT to a file, specified
 *    in the file_name parameter. Must be followed by a restore method call to
 *    restore output or input.
 *
 * @par Class:
 *    Reroute
 *
 * @param[in] int file_no - Redirect STDIN, 0, or STDOUT, 1
 * @param[in] int input_pipe[] - pipe to redirect
 *
 * @returns bool - success or failure
 *
 ******************************************************************************/
bool Reroute::redirect(int file_no, string ip, string port)
{
   //Creae a socket for port and ip
   int sockfd = createSocket(ip, port);
   bool return_val; //Temporary storage for redirect return value

   //Check for a valid socket file descriptor
   if(sockfd < 0)
   {
      return false;
   }

   //Call redirect and return 
   return_val = redirect(file_no, sockfd); 
   return return_val;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function creates a socket for the specifed ip address and port.
 *    Modified version of the example code given in class.
 *
 * @par Class:
 *    Reroute
 *
 * @param[in] string ip 
 * @param[in] string port
 *
 * @returns int - socket file descriptor
 *
 ******************************************************************************/
int Reroute::createSocket(string ip, string port)
{
    int sockfd = 0;
    struct sockaddr_in serv_addr; 

    //Check if the port is an integer
    if(!isinteger(port))
    {
       cout << "Port must be an integer" << endl;
       return -1;
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "\n Error : Could not create socket \n" << endl;
        return -1;
    } 

   //Zero out serv_addr
    memset(&serv_addr, '0', sizeof(serv_addr)); 

    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(stringToInt(port)); 

    if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)<=0)
    {
        cout << "\n inet_pton error occured" << endl;
        return -1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       cout << "\n Error : Connect Failed" << endl;
       return -1;
    } 

    return sockfd;
}
/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function creates a remote output socket for the specifed port. 
 *    Function call must be eventually followed with a restore call to return
 *    output to stdout
 *
 * @par Class:
 *    Reroute
 *
 * @param[in] int file_no - Redirect STDIN, 0, or STDOUT, 1
 * @param[in] string port - Port to output to
 *
 * @returns bool - success or failure
 *
 ******************************************************************************/
bool Reroute::redirect_remote(int file_no, string port)
{
   int remote_fd;   //File descriptor of remote connection
   bool return_val; //Temporary storage for redirect return value
   remote_fd = listenForSocket(port); // Wait for a connection

   //Check for a valid file descriptor
   if(remote_fd < 0)
   {
      cout << "Unable to open socket." << endl;
      return false;
   }

   //Redirect to this file descriptor, and close the useless descriptor
   return_val = redirect(file_no, remote_fd);
   close(remote_fd);
   return return_val;
}

/***************************************************************************//**
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This function creates a server socket listener file descriptor in order
 *    to output to a client application. It listens on the specified port.
 *
 *    Modified for example code given in class
 *
 * @par Class:
 *    Reroute
 *
 * @param[in] string port - Port to output to
 *
 * @returns int - connection file descriptor
 *
 ******************************************************************************/
int Reroute::listenForSocket(string port)
{
   int listenfd = 0, connfd = 0;
   int sockopt = 1;
    struct sockaddr_in serv_addr; 

   //Check if port is an integer
    if(!isinteger(port))
    {
       cout << "Port must be an integer" << endl;
       return -1;
    }


    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(int));

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(stringToInt(port)); 

    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    { 
       perror("Error");
       cout << endl;
       return -1;
    }

   listen(listenfd, 10); 

   //Wait for a connection
   connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
   return connfd;
}
