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
*/

/* Include Micrium and STM headers. */
#include "includes.h"

/*----- c o n s t a n t    d e f i n i t i o n s -----*/
#define P1Char  0x03 /* preamble byte 1 */
#define P2Char  0xEF /* preamble byte 2 */
#define P3Char  0xAf /* preamble byte 3 */
#define HeaderLength 5
#define MinPacketLength 8 /* minimum packet size */
#define DesinationAddress 1 /* destination address */
#define DestAdressIndex 0
#define MessageIndex 2
#define MessageTypeLV 0x01
#define MessageTypeHV 0x08

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void ParsePkt ( void *payloadBfr);
#endif