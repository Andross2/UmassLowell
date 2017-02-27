/*------ F I L E : S e r I O D r i v e r .c ------

by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
To handle transmitter and reciever

CHANGES
02-17-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-2
*/
/* include service output module header*/
#include "SerIODriver.h"

// Use deault buffer size of 4
#ifndef BfrSize
#define BfrSize 4
#endif

#define USART_TXE   0x80 /* Transmitter bit mask */
#define USART_RXNE  0x20 /* Receiver bit mask */

//Allocate the input buffer pair
static BfrPair iBfrPair;
static CPU_INT08U iBfr0Space[BfrSize];
static CPU_INT08U iBfr1Space[BfrSize];

//Allocate the output buffer pair
static BfrPair oBfrPair;
static CPU_INT08U oBfr0Space[BfrSize];
static CPU_INT08U oBfr1Space[BfrSize];

/*--------------- F U N C T I O N : I n i t S e r I O ( ) ---------------

PURPOSE
To intialize input buffer pair
and output buffer pair

INPUT PARAMETERS
no value

RETURN VALUE
no value
*/

void InitSerIO(void)
{
  BfrPairInit( &iBfrPair,iBfr0Space,iBfr1Space,BfrSize);
  BfrPairInit( &oBfrPair,oBfr0Space,oBfr1Space,BfrSize);
}

/*--------------- F U N C T I O N : P u t B y t e ( ) ---------------

PURPOSE
To put byte to output buffer

INPUT PARAMETERS
the byte

RETURN VALUE
the byte or -1
*/

CPU_INT16S PutByte(CPU_INT16S txChar)
{
  if(BfrPairSwappable(&oBfrPair))
  {
    BfrPairSwap(&oBfrPair);
  }
  if(PutBfrClosed(&oBfrPair))
    return -1;
  return PutBfrAddByte(&oBfrPair, txChar);
}

/*--------------- F U N C T I O N : S e r v i c e T x ( ) ---------------

PURPOSE
To transmit the data

INPUT PARAMETERS
no value

RETURN VALUE
no value
*/

void ServiceTx(void)
{
  if(!(USART2->SR & USART_TXE))
  {
    return;
  }
  if(!GetBfrClosed(&oBfrPair))
  {
    return ;
  }
  USART2->DR = GetBfrRemByte(&oBfrPair);
}

/*--------------- F U N C T I O N : G e t B y t e ( ) ---------------

PURPOSE
To get byte from input buffer 

INPUT PARAMETERS
the byte

RETURN VALUE
the byte or -1
*/

CPU_INT16S GetByte(void)
{
  if( BfrPairSwappable(&iBfrPair))
  {
    BfrPairSwap(&iBfrPair);
  }
  if(!GetBfrClosed(&iBfrPair))
    return -1;
  return GetBfrRemByte(&iBfrPair);
}

/*--------------- F U N C T I O N : S e r v i c e R x ( ) ---------------

PURPOSE
To recieve the data

INPUT PARAMETERS
no value

RETURN VALUE
no value
*/

void ServiceRx(void)
{
  if(!(USART2->SR & USART_RXNE))
  {
    return;
  }
  if(PutBfrClosed(&iBfrPair))
  {
    return ;
  }
  PutBfrAddByte(&iBfrPair,(CPU_INT16S) USART2->DR);
}