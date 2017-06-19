/*------ F I L E : B f r P a i r .h ------*/
#ifndef BfrPair_H
#define BfrPair_H
/*
by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
This header file defines the public names (functions and types)
exported from the module "BfrPair.c."

CHANGES
02-17-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-2
*/

/* Include Micrium and STM headers. */
#include "includes.h"
#include "Buffer.h"

#define NumBfrs 2 /* No of buffers */

/*----- t y p e    d e f i n i t i o n s -----*/
/*
Buffer Pair
*/
typedef struct
{
  CPU_INT08U putBfrNum;
  Buffer buffer[NumBfrs];
} BfrPair;

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void BfrPairInit(BfrPair *bfrPair, CPU_INT08U *bfr0Space, CPU_INT08U *bfr1Space, CPU_INT16U size);
void PutBfrReset(BfrPair *bfrpair);
CPU_INT08U *PutBfrAddr(BfrPair *bfrPair);
CPU_INT08U *GetBfrAddr(BfrPair *bfrPair);
CPU_INT08U PutBfrClosed(BfrPair *bfrPair);
CPU_INT08U GetBfrClosed(BfrPair *bfrPair);
void ClosePutBfr(BfrPair *bfrpair);
void OpenGetBfr(BfrPair *bfrpair);
CPU_INT16S PutBfrAddByte(BfrPair *bfrpair, CPU_INT16S byte);
CPU_INT16S GetBfrNextByte(BfrPair *bfrpair);
CPU_INT16S GetBfrRemByte(BfrPair *bfrpair);
CPU_BOOLEAN BfrPairSwappable(BfrPair *bfrpair);
void BfrPairSwap(BfrPair *bfrpair);
#endif