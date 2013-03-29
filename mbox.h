#ifndef __MBOX_H_
#define __MBOX_H_
/***************************************************************************//**
 * @file mbox.h
 * @author Hayden Waisanen
 *
 * @par Description:
 *    This class handles operations associated with inter-dsh communication, 
 *    mail boxes. Using the Semaphores and Shrmem classes, memory is protected
 *    from various problems associated with the readers-writers problem, 
 *    including starvation and deadlock.
 *
 * @par Class:
 *    Mbox
 *
 ******************************************************************************/

/***************************************************************************//**
 * Includes
 ******************************************************************************/
#include "helper.h"
#include<string>
#include<iostream>
#include<vector>
#include "Semaphore.h"
#include "Shrmem.h"

using namespace std;


/***************************************************************************//**
 * Enums
 ******************************************************************************/
/**
 * Readers-writers Enum
 * This enums map to various semaphores associated with a particular block of
 * shared memory 
 */
enum {
   RW,      /*!< maps to Read-Write semaphore for a block */
   READERS, /*!< maps to Readers semaphore for a block */
   MUTEX    /*!< maps to Readers_count mutex semaphore for a block */
   };

class Mbox {
public:
   //Constructors/Destructors
   Mbox();
   ~Mbox();

   //Public Methods
   void init(vector<string> params);
   void del(vector<string> params);
   void write(vector<string> params);
   void read(vector<string> params);
   void copy(vector<string> params);
private:
   //Data members
   Semaphores * sem;
   Shrmem * shrmem;
   bool creator;

   //Methods
   string lockandread(int box);
   void lockandwrite(int box, string input);
};
#endif
