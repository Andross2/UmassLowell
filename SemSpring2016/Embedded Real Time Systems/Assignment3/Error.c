/*-----------------------FILE : Error.c---------------------

by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
To Display Error Type of the Bytes received

CHANGES
01-30-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-1
02-17-2016 - changes made for Assignment-2
*/

/* Include Error Message Module header*/
#include "Error.h"

/*--------------- F U N C T I O N : A p p M a i n ( ) ---------------

PURPOSE
To display the error in the recieved byte from Rx

INPUT PARAMETERS
error number

RETURN VALUE
error type
*/

void ErrorType(CPU_INT08S errorNumber, CPU_INT08U *msgPtr)
{
   switch(errorNumber)
    {
    case (P1Error) : 
      sprintf((CPU_CHAR *) msgPtr,"\a*** ERROR: Bad Preamble Byte 1 \n");
      break;
      
    case (P2Error) : 
      sprintf((CPU_CHAR *) msgPtr,"\a*** ERROR: Bad Preamble Byte 2 \n");
      break;
 
    case (P3Error) :
      sprintf((CPU_CHAR *) msgPtr,"\a*** ERROR: Bad Preamble Byte 3 \n");
      break;
    
    case (PaketLengthError) : 
      sprintf((CPU_CHAR *) msgPtr,"\a*** ERROR: Bad Packet Size \n");
      break;
      
    case (CheckSumError) : 
      sprintf((CPU_CHAR *) msgPtr,"\a*** ERROR: Checksum error \n");
      break;
 
    case (DestinationAddressError) :
      sprintf((CPU_CHAR *) msgPtr,"\a*** INFO: Not My Address \n");
      break;
 
    case (UnknownMessageError) :
      sprintf((CPU_CHAR *) msgPtr,"\a*** ERROR: Unknown Message Type \n");
      break;
    }
}

