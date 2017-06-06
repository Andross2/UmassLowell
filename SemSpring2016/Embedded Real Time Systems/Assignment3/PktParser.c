/*------------------- F I L E : P k t P a r s e r .c ----------------------

by:	Naga Ganesh Kurapati
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
Read one paket from the Rx, extracting and
returing the packet payload

CHANGES
01-30-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-1
02-17-2016 - changes made for Assignment-2
*/

/* Include Paket Parser and Error Message Module headers*/
#include "PktParser.h"
#include "Error.h"

/*----- c o n s t a n t    d e f i n i t i o n s -----*/
#define P1Char  0x03 /* preamble byte 1 */
#define P2Char  0xEF /* preamble byte 2 */
#define P3Char  0xAf /* preamble byte 3 */
#define HeaderLength 5
#define MinPacketLength 8 /* minimum packet size */

/*----- t y p e    d e f i n i t i o n s -----*/
/*
Paket buffer
*/
typedef struct 
{
  CPU_INT08S payloadLen; /* payload length */
  CPU_INT08U data[1]; /* data array */
} PktBfr;

/*----- t y p e    d e f i n i t i o n s -----*/
typedef enum { P1 , P2 , P3 , PLength , DataBytes , CheckSum , ER} ParserState ;

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void ErrorHandler (PktBfr *pktBfr,BfrPair *payloadBfrPair,CPU_INT16S errorNumber);

/*--------------- F U N C T I O N : P a r s e P k t ( ) ---------------

PURPOSE
Read one paket from the Rx, extracting and
returing the packet payload

INPUT PARAMETERS
variable points to payload

RETURN VALUE
no value
*/
void ParsePkt (BfrPair *payloadBfrPair)
{
  static ParserState parseState = P1; /* to represent the parser state */
  CPU_INT16S c; /* to read the bytes from Rx */
  static CPU_INT08U i=0; /* data index */
  static CPU_INT16S CheckSumXor; /* to read checksum value, Xor of all the bytes*/
  PktBfr *pktBfr = (PktBfr *) PutBfrAddr(payloadBfrPair); /* to make payloadBfr and pktBfr look alike*/
  
  if(PutBfrClosed(payloadBfrPair))
    {
      return;
    }
  for(;;)
  {
    c = GetByte(); /* to read the bytes from Rx */
    if(c < 0)
    {
      return ;
    }
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
        ErrorHandler(pktBfr,payloadBfrPair,P1Error);
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
        ErrorHandler(pktBfr,payloadBfrPair,P2Error);
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
        ErrorHandler(pktBfr,payloadBfrPair,P3Error);
      }
      break;
      
      /* to read, check paket length byte */
    case PLength :
      if ( c < MinPacketLength )
      {
        parseState = ER;
        ErrorHandler(pktBfr,payloadBfrPair,PaketLengthError);
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
       ErrorHandler(pktBfr,payloadBfrPair,CheckSumError);
      }
      else
      {
       parseState = P1;
       ClosePutBfr(payloadBfrPair);
       return;
      }
      break;
      
      /* to read, check until valid sync bytes received */
    case ER:
      if(c== P1Char)
      {
        CheckSumXor=c;
        c = GetByte();
        if(c < 0)
        {
          return;
        }
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

/*--------------- F U N C T I O N : E r r o r H a n d l e r ( ) ---------------

PURPOSE
Handling the errors and closing the payload buffer

INPUT PARAMETERS
variable points to payload, payload buffer, errortype

RETURN VALUE
no value
*/

void ErrorHandler (PktBfr *pktBfr,BfrPair *payloadBfrPair,CPU_INT16S errorNumber)
{
  pktBfr->payloadLen = errorNumber;
  ClosePutBfr(payloadBfrPair);
}
