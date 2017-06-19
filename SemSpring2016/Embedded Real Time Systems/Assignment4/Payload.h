/* ------ F I L E : P a y l o a d.h ------*/
#ifndef PayLoad_H_
#define PayLoad_H_
/*
by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
This header file defines the public names (functions and types)
exported from the module "PktParser.c."

CHANGES
01-30-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-1
02-17-2016 - changes made for Assignment-2
03-09-2016 - changes made for Assignment-4
*/

#include "BfrPair.h"
#include <assert.h>

/*----- g l o b a l    v a r i a b l e s -----*/
extern BfrPair payloadBfrPair;
extern OS_SEM openPayloadBfrs;
extern OS_SEM closedPayloadBfrs;

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void CreatePayloadTask(void);
#endif