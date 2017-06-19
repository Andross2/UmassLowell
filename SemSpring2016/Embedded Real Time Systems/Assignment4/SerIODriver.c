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
#include <assert.h>

// Use deault buffer size of 4
#ifndef BfrSize
#define BfrSize 4
#endif

/*----- c o n s t a n t    d e f i n i t i o n s -----*/
#define SETENA1 (*((CPU_INT32U *) 0XE000E104)) /* defining the Interrupt Enabling register */
#define USART2ENA 0x00000040 /* Mask bits for enabling Interrupts */
#define TxRxUnmask 0x20AC /* To enable Tx and Rx */
#define USART_RXNE  0x20 /* Receiver bit mask */
#define USART_RXNEIE  0x20 /* Receiver bit unmask */
#define USART_TXE   0x80 /* Transmitter bit mask */
#define USART_TXEIE   0x80 /* Transmitter bit unmask */

#define SuspendTimeout 0 // Timeout for semaphore wait
#define OpenOBfrsCount 2
#define ClosediBfrsCount 0

//----- g l o b a l    v a r i a b l e s -----

static OS_SEM openObfrs; /* open output buffer semaphore */
static OS_SEM closedIBfrs; /* closed input buffer semaphore */

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
  OS_ERR  osErr;
  
  BfrPairInit( &iBfrPair,iBfr0Space,iBfr1Space,BfrSize);
  BfrPairInit( &oBfrPair,oBfr0Space,oBfr1Space,BfrSize);
  
  OSSemCreate(&openObfrs, "Buffer Empty",OpenOBfrsCount, &osErr);	
  assert(osErr == OS_ERR_NONE);
  OSSemCreate(&closedIBfrs, "Buffer Full",ClosediBfrsCount, &osErr);
  assert(osErr == OS_ERR_NONE);
  
  USART2->CR1 = TxRxUnmask;
  SETENA1 = USART2ENA;
}

/*--------------- F U N C T I O N : P u t B y t e ( ) ---------------

PURPOSE
To put byte to output buffer

INPUT PARAMETERS
the byte

RETURN VALUE
the byte or -1
*/

CPU_INT16S PutByte(CPU_INT16S c)
{
  OS_ERR  osErr;
  if(PutBfrClosed(&oBfrPair))
  {
    OSSemPend(&openObfrs, SuspendTimeout, OS_OPT_PEND_BLOCKING, NULL, &osErr);
    assert(osErr==OS_ERR_NONE);
    BfrPairSwap(&oBfrPair);
  }
  PutBfrAddByte(&oBfrPair,c);
  USART2->CR1 |= USART_TXEIE;
  return c;
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
  OS_ERR  osErr;
  if(!(USART2->SR & USART_TXE))
  {
    return;
  }
  if(!GetBfrClosed(&oBfrPair))
  {
    USART2->CR1 &= ~USART_TXE;
    return ;
  }
  USART2->DR = GetBfrRemByte(&oBfrPair);
  if(!GetBfrClosed(&oBfrPair))
  {
    OSSemPost(&openObfrs, OS_OPT_POST_1, &osErr);
    assert(osErr==OS_ERR_NONE);
  }
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
  OS_ERR  osErr;
  CPU_INT16S c;
  if(!GetBfrClosed(&iBfrPair))
  {
    OSSemPend(&closedIBfrs, SuspendTimeout, OS_OPT_PEND_BLOCKING, NULL, &osErr);
    assert(osErr == OS_ERR_NONE);
    BfrPairSwap(&iBfrPair);
  }
  c = GetBfrRemByte(&iBfrPair);
  USART2->CR1 |= USART_RXNEIE;
  return c;
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
  OS_ERR  osErr;
  if(!(USART2->SR & USART_RXNE))
  {
    return;
  }
  if(PutBfrClosed(&iBfrPair))
  {
    USART2->CR1 &= ~USART_RXNE;
    return ;
  }
  PutBfrAddByte(&iBfrPair,USART2->DR);
  if(PutBfrClosed(&iBfrPair))
  {
    OSSemPost(&closedIBfrs, OS_OPT_POST_1, &osErr); 
    assert(osErr==OS_ERR_NONE);
  }
}

/*--------------- F U N C T I O N :  S e r i a l I S R ( ) ---------------

PURPOSE
Call ServiceRx() to handle Rx interrupts and
then call ServiceTx() to handle Tx interrupts.

INPUT PARAMETERS
no value

RETURN VALUE
no value
*/

void SerialISR(void)
{
  /* Disable interrupts. */
  CPU_SR_ALLOC();
  OS_CRITICAL_ENTER();  
  
  /* Tell kernel we're in an ISR. */
  OSIntEnter();
  
  /* Enable interrupts. */
  OS_CRITICAL_EXIT();
  
  ServiceRx();
  ServiceTx();
  
  /* Give the O/S a chance to swap tasks. */
  OSIntExit ();
  
}