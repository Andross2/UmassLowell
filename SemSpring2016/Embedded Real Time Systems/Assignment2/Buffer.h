/*------ F I L E : B u f f e r .h ------*/
#ifndef Buffer_H
#define Buffer_H
/*
by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
This header file defines the public names (functions and types)
exported from the module "Buffer.c."

CHANGES
02-17-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-2
*/

/* Include Micrium and STM headers. */
#include "includes.h"

/*----- t y p e    d e f i n i t i o n s -----*/
/*
Buffer
*/
typedef struct
{
  CPU_BOOLEAN closed; /* True if buffer has data ready to process */
  CPU_INT16U size; /* The capacity of the buffer in bytes */
  CPU_INT16U putIndex; /* The Position where the next byte will be added */
  CPU_INT16U getIndex; /* The position of the next bte to remove */
  CPU_INT08U *buffer; /* The address of the buffer space */
} Buffer;

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void BfrInit (Buffer *bfr, CPU_INT08U *bfrSpace, CPU_INT16U size);
void BfrReset (Buffer *bfr);
CPU_BOOLEAN BfrClosed (Buffer *bfr);
void BfrClose (Buffer *bfr);
void BfrOpen (Buffer *bfr);
CPU_BOOLEAN BfrFull(Buffer *bfr);
CPU_BOOLEAN BfrEmpty(Buffer *bfr);
CPU_INT16S BfrAddByte(Buffer *bfr, CPU_INT16S theByte);
CPU_INT16S BfrNextByte(Buffer *bfr);
CPU_INT16S BfrRemoveByte(Buffer *bfr);

#endif