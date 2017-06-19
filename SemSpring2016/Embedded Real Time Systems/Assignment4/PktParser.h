/*---------------------- F I L E : P k t P a r s e r .h ---------------------*/
#ifndef PktParser_H
#define PktParser_H

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
*/

/* Include Micrium and STM headers. */
#include "includes.h"
#include "BfrPair.h"
#include "SerIODriver.h"
#include "Payload.h"
#include <assert.h>

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void CreatePktParserTask(void);
#endif