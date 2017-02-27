/*-----------------------FILE : Error.c---------------------*/

/*
by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
To Display Error Type of the Bytes received

CHANGES
01-30-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-1
*/

/* Include Error Message Module header*/
#include "Error.h"

/*--------------- F U N C T I O N : A p p M a i n ( ) ---------------*/

/*
PURPOSE
To display the error in the recieved byte from Rx

INPUT PARAMETERS
error number

RETURN VALUE
error type
*/

void ErrorType(CPU_INT08U errorNumber)
{
   switch(errorNumber)
    {
    case P1Error : 
      BSP_Ser_Printf("\a*** ERROR: Bad Preamble Byte 1 \n");
      break;
      
    case P2Error : 
      BSP_Ser_Printf("\a*** ERROR: Bad Preamble Byte 2 \n");
      break;
 
    case P3Error :
      BSP_Ser_Printf("\a*** ERROR: Bad Preamble Byte 3 \n");
      break;
    
    case PaketLengthError : 
      BSP_Ser_Printf("\a*** ERROR: Bad Packet Size \n");
      break;
      
    case CheckSumError : 
      BSP_Ser_Printf("\a*** ERROR: Checksum error \n");
      break;
 
    case DestinationAddressError :
      BSP_Ser_Printf("\a*** INFO: Not My Address \n");
      break;
 
    case UnknownMessageError :
      BSP_Ser_Printf("\a*** ERROR: Unknown Message Type \n");
      break;
    }
}

