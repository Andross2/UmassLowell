/*----------------------- F I L E : E r r o r .h---------------------*/
#ifndef Error_H
#define Error_H

/*
by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
This header file defines the public names (functions and types)
exported from the module "Error.c."

CHANGES
01-30-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-1
*/

/* Include Micrium and STM headers. */
#include "includes.h"

/*----- t y p e    d e f i n i t i o n s -----*/
/*
enumeration of error types
*/

enum { P1Error , P2Error , P3Error , PaketLengthError , CheckSumError , DestinationAddressError , UnknownMessageError} ;

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void ErrorType(CPU_INT08U errorNumber);
#endif