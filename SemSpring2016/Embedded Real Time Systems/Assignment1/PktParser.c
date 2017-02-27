/*------------------- F I L E : P k t P a r s e r .c ----------------------*/

/*
by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
Read one paket from the Rx, extracting and
returing the packet payload

CHANGES
01-30-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-1
*/

/* Include Paket Parser and Error Message Module headers*/
#include "PktParser.h"
#include "Error.h"

/*----- t y p e    d e f i n i t i o n s -----*/
typedef enum { P1 , P2 , P3 , PLength , DataBytes , CheckSum , ER} ParserState ;

/*
Paket buffer
*/
typedef struct 
{
  CPU_INT08U payloadLen;
  CPU_INT08U data[1];
} PktBfr;

/*--------------- F U N C T I O N : P a r s e P k t ( ) ---------------*/
/*
PURPOSE
Read one paket from the Rx, extracting and
returing the packet payload

INPUT PARAMETERS
variable points to payload

RETURN VALUE
no value
*/
void ParsePkt ( void *payloadBfr)
{
  ParserState parseState = P1; /* to represent the parser state */
  CPU_INT16S c; /* to read the bytes from Rx */
  CPU_INT08U i=0; /* data index */
  CPU_INT16S CheckSumXor; /* to read checksum value, Xor of all the bytes*/
  PktBfr *pktBfr = payloadBfr; /* to make payloadBfr and pktBfr look alike*/
  for(;;)
  {
    c = GetByte(); /* to read the bytes from Rx */
    switch (parseState)
    {
      
      /* to read, check preamble byte 1 */
    case P1 :
      if (c == P1Char)
      {
        parseState = P2;
        CheckSumXor = c;
      }
      else
      {
        parseState = ER;
        ErrorType (P1Error);
      }
      break;
      
      /* to read, check preamble byte 2 */
    case P2 :
      if (c == P2Char)
      {
        parseState = P3;
        CheckSumXor = CheckSumXor ^ c;
      }
      else
      {
        parseState = ER;
        ErrorType (P2Error);
      }
      break;
      
      /* to read,check preamble byte 3 */
    case P3 :
      if (c == P3Char)
      {
        parseState = PLength;
        CheckSumXor = CheckSumXor ^ c;
      }
      else
      {
        parseState = ER;
        ErrorType (P3Error);
      }
      break;
      
      /* to read, check paket length byte */
    case PLength :
      if ( c < MinPacketLength )
      {
        parseState = ER;
        ErrorType (PaketLengthError);
      }
      else
      {
        pktBfr->payloadLen = c - HeaderLength;
        parseState = DataBytes;
        CheckSumXor = CheckSumXor ^ c;
        i=0;
      }
      break;
      
      /* read the data bytes*/
    case DataBytes :
      CheckSumXor = CheckSumXor ^ c;
      pktBfr->data[i++]=c;
      if(i >= pktBfr->payloadLen)
      {
        parseState = CheckSum;
      }
      break;
      
      /* to read, check checksum byte */
    case CheckSum:
      CheckSumXor = CheckSumXor ^ c; /* to compute checksum, xor of all bytes */
      if (!(CheckSumXor==0)) /* error if xor all bytes with checksum byte is not equal to zero*/
      {
       parseState = ER;
       ErrorType (CheckSumError);
      }
      else
      {
       parseState = P1;
       return;
      }
      break;
      
      /* to read, check until valid sync bytes received */
    case ER:
      if(c== P1Char)
      {
        CheckSumXor=c;
        c = GetByte();
        if (c == P2Char)
       {
        parseState = P3;
        CheckSumXor = CheckSumXor ^ c;
       }
      }
      break;
    }
  }
}