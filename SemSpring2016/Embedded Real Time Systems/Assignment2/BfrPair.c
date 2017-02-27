/*------ F I L E : B f r P a i r .c ------

by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
To handle Buffer Pair

CHANGES
02-17-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-2
*/
/* include buffer pair module header */
#include "BfrPair.h"
/*--------------- F U N C T I O N : B f r P a i r I n i t ( ) ---------------

PURPOSE
To intialize both buffers of the buffer pair

INPUT PARAMETERS
buffer pair address, address of the buffer 0 space,
address of the buffer 0 space, buffer capacity in bytes

RETURN VALUE
none
*/

void BfrPairInit(BfrPair *bfrPair, CPU_INT08U *bfr0Space, CPU_INT08U *bfr1Space, CPU_INT16U size)
{
  bfrPair->putBfrNum =0;
  BfrInit (&bfrPair->buffer[0], bfr0Space, size);
  BfrInit (&bfrPair->buffer[1], bfr1Space, size);
}
/*--------------- F U N C T I O N : P u t B f r R e s e t ( ) ---------------

PURPOSE
To reset the put buffer

INPUT PARAMETERS
buffer pair address

RETURN VALUE
none
*/

void PutBfrReset(BfrPair *bfrPair)
{
  BfrReset (&bfrPair->buffer[bfrPair->putBfrNum]);
}
/*--------------- F U N C T I O N : *P u t B f r A d d r ( ) ---------------

PURPOSE
Obtain the address of the put buffer's
buffer data space

INPUT PARAMETERS
buffer pair address

RETURN VALUE
The address of the put buffer's buffer data space
*/

CPU_INT08U *PutBfrAddr(BfrPair *bfrPair)
{
  return(bfrPair->buffer[bfrPair->putBfrNum].buffer);
}
/*--------------- F U N C T I O N : *G e t B f r A d d r ( ) ---------------

PURPOSE
Obtain the address of the get buffer's
buffer data space

INPUT PARAMETERS
buffer pair address

RETURN VALUE
The address of the get buffer's buffer data space
*/

CPU_INT08U *GetBfrAddr(BfrPair *bfrPair)
{
  return(bfrPair->buffer[1 - bfrPair->putBfrNum].buffer);
}
/*--------------- F U N C T I O N : P u t B f r C l o s e d ( ) ---------------

PURPOSE
To test whether or not a put buffer is closed

INPUT PARAMETERS
buffer pair address

RETURN VALUE
TRUE if closed, otherwise FALSE
*/

CPU_INT08U PutBfrClosed(BfrPair *bfrPair)
{
  return (BfrClosed(&bfrPair->buffer[bfrPair->putBfrNum]));
}
/*--------------- F U N C T I O N : G e t B f r C l o s e d ( ) ---------------

PURPOSE
To test whether or not a get buffer is closed

INPUT PARAMETERS
buffer pair address

RETURN VALUE
TRUE if closed, otherwise FALSE
*/

CPU_INT08U GetBfrClosed(BfrPair *bfrPair)
{
  return (BfrClosed(&bfrPair->buffer[1 - bfrPair->putBfrNum]));
}
/*--------------- F U N C T I O N : C l o s e P u t B f r ( ) ---------------

PURPOSE
To mark the put buffer closed

INPUT PARAMETERS
buffer pair address

RETURN VALUE
none
*/

void ClosePutBfr(BfrPair *bfrPair)
{
  BfrClose(&bfrPair->buffer[bfrPair->putBfrNum]);
}
/*--------------- F U N C T I O N : O p e n G e t B f r ( ) ---------------

PURPOSE
To mark the get buffer open

INPUT PARAMETERS
buffer pair address

RETURN VALUE
none
*/

void OpenGetBfr(BfrPair *bfrPair)
{
  BfrOpen(&bfrPair->buffer[1 - bfrPair->putBfrNum]);
}
/*--------------- F U N C T I O N : P u t B f r A d d B y t e ( ) ---------------

PURPOSE
Add byte to a buffer at position "putindex" and
increment "putindex" by 1
If the buffer becomes full, mark it closed

INPUT PARAMETERS
buffer pair address, byte to be added

RETURN VALUE
The byte added, unless the buffer was full.
If the buffer was full, return -1.
*/

CPU_INT16S PutBfrAddByte(BfrPair *bfrPair, CPU_INT16S byte)
{
  return (BfrAddByte(&bfrPair->buffer[bfrPair->putBfrNum], byte));
}
/*--------------- F U N C T I O N : G e t B f r N e x t B y t e ( ) ---------------

PURPOSE
Return the byte from position "getIndex" or return -1
if the buffer is empty

INPUT PARAMETERS
buffer pair address

RETURN VALUE
The byte from position "grtIndex" unless
the buffer is empty. If the buffer is empty, return -1
*/

CPU_INT16S GetBfrNextByte(BfrPair *bfrPair)
{
  return (BfrNextByte(&bfrPair->buffer[1- bfrPair->putBfrNum]));
}
/*--------------- F U N C T I O N : G e t B f r R e m B y t e ( ) ---------------

PURPOSE
Return the byte from position "getIndex" and increment "getIndex" by 1.
If the buffer is becomes empty, mark it open

INPUT PARAMETERS
buffer pair address

RETURN VALUE
The byte from position "grtIndex" unless
the buffer is empty. If the buffer is empty, return -1
*/

CPU_INT16S GetBfrRemByte(BfrPair *bfrPair)
{
  return (BfrRemoveByte(&bfrPair->buffer[1- bfrPair->putBfrNum]));
}
/*--------------- F U N C T I O N : B f r P a i r S w a p p a b l e ( ) ---------------

PURPOSE
Test whether or not a buffer pair is ready to be swapped.
It is ready if the put buffer is closed and the get buffer is open

INPUT PARAMETERS
buffer pair address

RETURN VALUE
TRUE if ready to swap, otherwise FALSE
*/

CPU_BOOLEAN BfrPairSwappable(BfrPair *bfrpair)
{
  return(PutBfrClosed(bfrpair)&&(!GetBfrClosed(bfrpair)));
}
/*--------------- F U N C T I O N : B f r P a i r S w a p p a b l e ( ) ---------------

PURPOSE
Swap the put buffer and the get buffer and reset the put buffer

INPUT PARAMETERS
buffer pair address

RETURN VALUE
None
*/

void BfrPairSwap(BfrPair *bfrpair)
{
  bfrpair->putBfrNum = 1- bfrpair->putBfrNum;
  PutBfrReset(bfrpair);
}