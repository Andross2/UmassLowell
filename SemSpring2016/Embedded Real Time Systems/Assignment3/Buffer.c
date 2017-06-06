/*------ F I L E : B u f f e r .c ------

by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
To handle Buffer

CHANGES
02-17-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-2
*/
/* Include buffer module header*/
#include "Buffer.h"

/*--------------- F U N C T I O N : B f r I n i t ( ) ---------------

PURPOSE
To intialize the buffer

INPUT PARAMETERS
buffer address, address of the buffer data space,
buffer capacity in bytes

RETURN VALUE
none
*/

void BfrInit (Buffer *bfr, CPU_INT08U *bfrSpace, CPU_INT16U size)
{
  bfr->closed = FALSE;
  bfr->size = size;
  bfr->putIndex = 0;
  bfr->getIndex = 0;
  bfr->buffer = bfrSpace;
}

/*--------------- F U N C T I O N : B f r R e s e t ( ) ---------------

PURPOSE
To reset the buffer

INPUT PARAMETERS
buffer address

RETURN VALUE
none
*/

void BfrReset (Buffer *bfr)
{
  bfr->closed = FALSE;
  bfr->putIndex = 0;
  bfr->getIndex = 0;
}

/*--------------- F U N C T I O N : B f r C l o s e d ( ) ---------------

PURPOSE
To test whether or not a buffer is closed

INPUT PARAMETERS
buffer address

RETURN VALUE
TRUE if closed, otherwise FALSE
*/


CPU_BOOLEAN BfrClosed (Buffer *bfr)
{
  return bfr->closed;
}

/*--------------- F U N C T I O N : B f r C l o s e ( ) ---------------

PURPOSE
To mark the buffer closed

INPUT PARAMETERS
buffer address

RETURN VALUE
none
*/


void BfrClose (Buffer *bfr)
{
  bfr->closed = TRUE;
}

/*--------------- F U N C T I O N : B f r O p e n ( ) ---------------

PURPOSE
To mark the buffer closed

INPUT PARAMETERS
buffer address

RETURN VALUE
none
*/

void BfrOpen (Buffer *bfr)
{
  bfr->closed = FALSE;
}

/*--------------- F U N C T I O N : B f r F u l l ( ) ---------------

PURPOSE
To test whether or not a buffer is full

INPUT PARAMETERS
buffer address

RETURN VALUE
TRUE if full, otherwise FALSE
*/

CPU_BOOLEAN BfrFull(Buffer *bfr)
{
  return (bfr->putIndex >= bfr->size);
}
/*--------------- F U N C T I O N : B f r E m p t y ( ) ---------------

PURPOSE
To test whether or not a buffer is empty

INPUT PARAMETERS
buffer address

RETURN VALUE
TRUE if empty, otherwise FALSE
*/

CPU_BOOLEAN BfrEmpty(Buffer *bfr)
{
  return (bfr->getIndex >= bfr->putIndex);
}
/*--------------- F U N C T I O N : B f r A d d B y t e ( ) ---------------

PURPOSE
Add byte to a buffer at position "putindex" and
increment "putindex" by 1
If the buffer becomes full, mark it closed

INPUT PARAMETERS
buffer address, byte to be added

RETURN VALUE
The byte added, unless the buffer was full.
If the buffer was full, return -1.
*/

CPU_INT16S BfrAddByte(Buffer *bfr, CPU_INT16S theByte)
{ 
  if(BfrFull(bfr))
  {
    return -1;
  }
  bfr->buffer[bfr->putIndex++]=theByte;
  if(BfrFull(bfr))
  {
    BfrClose (bfr);
  }
  return (theByte);
}
/*--------------- F U N C T I O N : B f r N e x t B y t e ( ) ---------------

PURPOSE
Return the byte from position "getIndex" or return -1
if the buffer is empty

INPUT PARAMETERS
buffer address

RETURN VALUE
The byte from position "grtIndex" unless
the buffer is empty. If the buffer is empty, return -1
*/

CPU_INT16S BfrNextByte(Buffer *bfr)
{
  if(BfrEmpty(bfr))
  {
    return -1;
  }
  return (bfr->buffer[bfr->getIndex]);
}
/*--------------- F U N C T I O N : B f r R e m o v e B y t e ( ) ---------------

PURPOSE
Return the byte from position "getIndex" and increment "getIndex" by 1.
If the buffer is becomes empty, mark it open

INPUT PARAMETERS
buffer address

RETURN VALUE
The byte from position "grtIndex" unless
the buffer is empty. If the buffer is empty, return -1
*/

CPU_INT16S BfrRemoveByte(Buffer *bfr)
{
  CPU_INT08U theByte;
  if(BfrEmpty(bfr))
  {
    return -1;
  }
  theByte = bfr->buffer[bfr->getIndex++];
  if(BfrEmpty(bfr))
  {
    BfrOpen (bfr);
  }
  return (theByte);
}