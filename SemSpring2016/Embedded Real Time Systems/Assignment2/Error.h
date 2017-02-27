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
02-17-2016 - changes made for Assignment-2
*/

/* Include Micrium and STM headers. */
#include "includes.h"

/*----- c o n s t a n t    d e f i n i t i o n s -----*/
#define P1Error -1
#define P2Error -2
#define P3Error -3
#define PaketLengthError -4
#define CheckSumError -5
#define DestinationAddressError -6
#define UnknownMessageError -7

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void ErrorType(CPU_INT08S errorNumber, CPU_INT08U *msgPtr);
#endif