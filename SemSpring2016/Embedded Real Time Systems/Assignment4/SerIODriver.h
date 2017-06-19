/*------ F I L E : S e r I O D r i v e r .h ------*/
#ifndef SerIODriver_H
#define SerIODriver_H

/*
by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
This header file defines the public names (functions and types)
exported from the module "SerIODriver.c."

CHANGES
02-17-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-2
*/

/* Include Micrium and STM headers. */
#include "includes.h"
#include "BfrPair.h"

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void InitSerIO(void);
CPU_INT16S PutByte(CPU_INT16S txChar);
void ServiceTx(void);
CPU_INT16S GetByte(void);
void ServiceRx(void);
void SerialISR(void);

#endif
