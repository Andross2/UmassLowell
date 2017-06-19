/* ------ F I L E : P a y l o a d.c ------

by:	Naga Ganesh Kurapati
Embedded Real Time Systems
Electrical and Computer Engineering Dept.
UMASS Lowell

PURPOSE
Display the decoded messages from the PayLoad

CHANGES
01-30-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-1
02-17-2016 - changes made for Assignment-2
*/

#include "PayLoad.h"
#include "Error.h"
#include "SerIODriver.h"
/*----- c o n s t a n t    d e f i n i t i o n s -----*/
#define SpeedLength 2 /* wind speed array length */
#define DepthLength 2 /* precipitation depth array length */
#define DataLength 10 /* id array length */
#define DesinationAddress 1 /* destination address */

#define PrecipitationLsb 1 /* precipitation message lsb index*/
#define PrecipitationMsb 0 /* precipitation message msb index*/
#define WindMsb 0 /* wind message msb index*/
#define WindLsb 1 /* wind message lsb index*/

#define SuspendTimeout 0	/* Timeout for semaphore wait */
#define PAYLOAD_STK_SIZE   128  /* PktParser stack size */
#define PayloadPrio        3    /* Payload task Priority */
#define OpenPBufrsCount 2
#define ClosedPBufrsCount 0

#define PayLoadBfrSize 16 /* Payload buffer size */
#define MsgBfrSize 160 /* Message buffer size */ 

/*----- t y p e    d e f i n i t i o n s -----*/
/*
enumeration of message type
*/
enum {TemperatureMessage = 1 , PressureMessage , HumidityMessage , WindMessage , 
RadiationMessage ,DateTimeMessage ,PrecipitationMessage ,IdMessage} ;

/*
PayLoad Structure
*/
#pragma pack(1)
typedef struct
{
  CPU_INT08S    payloadLen ; /* Payload length */
  CPU_INT08U    dstAddr ;  /* Destination address */
  CPU_INT08U    srcAddr ; /* Source address */
  CPU_INT08U    msgType ; /* Message type */
  union
  { 
    CPU_INT08S temp ; /* temperature */
    CPU_INT16U pres ;  /* pressure */
    struct
    {
      CPU_INT08S dewPt ; /* Dew point */
      CPU_INT08U hum ; /* humidity */
    } hum ;
    struct
    {
      CPU_INT08U speed[SpeedLength] ; /* wind speed */
      CPU_INT16U dir ; /* wind direction */
    } wind ;
    CPU_INT16U rad ; /* solar radiation */
    CPU_INT32U dateTime ; /* date and time */
    CPU_INT08U depth[DepthLength] ; /* Precipitation depth */
    CPU_INT08U id[DataLength] ; /* Sensor Id message */
  } dataPart ; 
} Payload ;

/*----- g l o b a l    v a r i a b l e s -----*/
static  OS_TCB   PayloadTCB;                     // Payload task TCB
static  CPU_STK  PayloadStk[PAYLOAD_STK_SIZE];  // Space for payoad task stack

// Allocate the payload buffer pair.
BfrPair payloadBfrPair;
CPU_INT08U pBfr0Space[PayLoadBfrSize];
CPU_INT08U pBfr1Space[PayLoadBfrSize];

OS_SEM openPayloadBfrs;
OS_SEM closedPayloadBfrs;
/*------- F u n c t i o n P r o t o t y p e s---------*/
void PayloadTask(void *);
void PayloadInit(BfrPair *pBfrPair);
CPU_INT32U Mask(CPU_INT08U width);
CPU_INT32U Extract(CPU_INT32U value, CPU_INT08U lsb, CPU_INT08U msb);
CPU_INT32U Insert(CPU_INT32U value, CPU_INT32U valInsert, CPU_INT08U lsb, CPU_INT08U msb);
void  PutReplyMsg(CPU_INT08U *msg);

/*--------------- F U N C T I O N : P a y l o a d I n i t ( ) ---------------

PURPOSE
Intialization of payload buffer pair

INPUT PARAMETERS
variable points to address of payload buffer pair

RETURN VALUE
no value
*/
void PayloadInit(BfrPair *pBfrPair)
{
  BfrPairInit( pBfrPair,pBfr0Space,pBfr1Space,PayLoadBfrSize);
}
/*--------------- C r e a t e P a y l o a d T a s k ( ) ---------------*/

/*
PURPOSE
Create the Payload task, initialize semaphores.

*/
void CreatePayloadTask(void)
{
  OS_ERR  osErr;
  
  /* Create the consumer task */
  OSTaskCreate(&PayloadTCB,            // Task Control Block
               "Payload Task",         // Task name
               PayloadTask,                // Task entry point
               NULL,                    // Address of optional task data block
               PayloadPrio,            // Task priority
               &PayloadStk[0],         // Base address of task stack space
               PAYLOAD_STK_SIZE / 10,  // Stack water mark limit
               PAYLOAD_STK_SIZE,       // Task stack size
               0,                 // This task has no task queue
               0,                       // Number of clock ticks (defaults to 10)
               NULL,          // Pointer to TCB extension
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), // Task options
               &osErr);                 // Address to return O/S error code
  
  assert(osErr == OS_ERR_NONE);
  
  OSSemCreate(&openPayloadBfrs, "Payload Empty",OpenPBufrsCount, &osErr);
  assert(osErr == OS_ERR_NONE);
  
  OSSemCreate(&closedPayloadBfrs,"Payload Full",ClosedPBufrsCount, &osErr);
  assert(osErr == OS_ERR_NONE);
  
  PayloadInit(&payloadBfrPair);
}

/*--------------- F U N C T I O N : P a y l o a d T a s k( ) ---------------

PURPOSE
Printing the data to message buffer

INPUT PARAMETERS
no value

RETURN VALUE
no value
*/
void PayloadTask(void *data)
{
  CPU_INT32U dateTimeMsg = 0; /* to read date and time message */
  CPU_INT08U msgBfr[MsgBfrSize];
  Payload *payload;
  for(;;)
  {
    if(!GetBfrClosed(&payloadBfrPair))
    {
      OS_ERR osErr;
      OSSemPend(&closedPayloadBfrs, SuspendTimeout, OS_OPT_PEND_BLOCKING, NULL, &osErr);
      assert(osErr==OS_ERR_NONE);
      //if(BfrPairSwappable(&payloadBfrPair))
      BfrPairSwap(&payloadBfrPair);
    }
    payload = (Payload *) GetBfrAddr(&payloadBfrPair);
    
    if(GetBfrClosed(&payloadBfrPair))
    {
      if(payload->payloadLen < 0 ) /* display error for negative payload length */
      {
        ErrorType(payload->payloadLen,msgBfr);
      }
      else
      {
        if(payload->dstAddr == DesinationAddress) /* executes the only if the desination address matches */
        {
          switch (payload->msgType)
          {
            
            /* to diplay temperature message */
          case TemperatureMessage : 
            sprintf((CPU_CHAR *) msgBfr,"\nSOURCE NODE %d: TEMPERATURE MESSAGE\n \tTemperature = %d\n",payload->srcAddr,payload->dataPart.temp);
            break;
            
            /* to diplay pressure message */
          case PressureMessage : 
            sprintf((CPU_CHAR *) msgBfr,"\nSOURCE NODE %d: BAROMETRIC PRESSURE MESSAGE\n \tPressure = %d\n",payload->srcAddr,
                    Insert(Insert(payload->dataPart.pres,Extract(payload->dataPart.pres,8,15),0,7),Extract(payload->dataPart.pres,0,7),8,15));
            break;
            
            /* to diplay dew point and humidity message */
          case HumidityMessage :
            sprintf((CPU_CHAR *) msgBfr,"\nSOURCE NODE %d: HUMIDITY MESSAGE\n \tDew Point = %d Humidity = %d\n",payload->srcAddr,
                    payload->dataPart.hum.dewPt,payload->dataPart.hum.hum);
            break;
            
            /* to diplay wind speed and direction message */
          case WindMessage :
            sprintf((CPU_CHAR *) msgBfr,"\nSOURCE NODE %d: WIND MESSAGE\n \tSpeed = %d%d%d.%d Wind Direction = %d\n", payload->srcAddr,
                    Extract(payload->dataPart.wind.speed[WindMsb],4,7),
                    Extract(payload->dataPart.wind.speed[WindMsb],0,3),
                    Extract(payload->dataPart.wind.speed[WindLsb],4,7),
                    Extract(payload->dataPart.wind.speed[WindLsb],0,3),
                    Insert(Insert(payload->dataPart.wind.dir,Extract(payload->dataPart.wind.dir,8,15),0,7),
                           Extract(payload->dataPart.wind.dir,0,7),8,15));
            break;
            
            /* to diplay solar radiation message */
          case RadiationMessage :
            sprintf((CPU_CHAR *) msgBfr,"\nSOURCE NODE %d: SOLAR RADIATION MESSAGE\n \tSolar Radiation Intensity = %d\n",payload->srcAddr,
                    Insert(Insert(payload->dataPart.rad,Extract(payload->dataPart.rad,8,15),0,7),
                           Extract(payload->dataPart.rad,0,7),8,15));
            break;
            
            /* to diplay date and time message */
          case DateTimeMessage :
            dateTimeMsg = Insert(dateTimeMsg,Extract(payload->dataPart.dateTime,24,31),0,7);
            dateTimeMsg = Insert(dateTimeMsg,Extract(payload->dataPart.dateTime,16,23),8,15);
            dateTimeMsg = Insert(dateTimeMsg,Extract(payload->dataPart.dateTime,8,15),16,23);
            dateTimeMsg = Insert(dateTimeMsg,Extract(payload->dataPart.dateTime,0,7),24,31);
            payload->dataPart.dateTime = dateTimeMsg;
            
            sprintf((CPU_CHAR *) msgBfr,"\nSOURCE NODE %d: DATE/TIME STAMP MESSAGE\n \tTime Stamp = %lu/%lu/%lu %lu:%lu\n", payload->srcAddr,
                    Extract(payload->dataPart.dateTime,5,8),
                    Extract(payload->dataPart.dateTime,0,4),
                    Extract(payload->dataPart.dateTime,9,20),
                    Extract(payload->dataPart.dateTime,27,31),
                    Extract(payload->dataPart.dateTime,21,26));
            break;
            
            /* to diplay precipitation depth message */
          case PrecipitationMessage :
            sprintf((CPU_CHAR *) msgBfr,"\nSOURCE NODE %d: PRECIPITATION MESSAGE\n \tPrecipitation Depth = %d%d.%d%d\n",payload->srcAddr,
                    Extract(payload->dataPart.depth[PrecipitationMsb],4,7),
                    Extract(payload->dataPart.depth[PrecipitationMsb],0,3),
                    Extract(payload->dataPart.depth[PrecipitationLsb],4,7),
                    Extract(payload->dataPart.depth[PrecipitationLsb],0,3));
            break;
            
            /* to diplay sensor id message */
          case IdMessage :
            sprintf((CPU_CHAR *) msgBfr,"\nSOURCE NODE %d: SENSOR ID MESSAGE\n\tNode ID = %s\n",payload->srcAddr,payload->dataPart.id);
            break;
            
            /* to diplay unknown message error */
          default : 
            ErrorType(UnknownMessageError,msgBfr);
            break;
          }
        }
        else /* display if destination address doesn't match */
        {
          ErrorType(DestinationAddressError,msgBfr);
        }
      }
      OS_ERR osErr;
      OpenGetBfr(&payloadBfrPair);
      OSSemPost(&openPayloadBfrs, OS_OPT_POST_1, &osErr);
      assert(osErr==OS_ERR_NONE);
      PutReplyMsg(msgBfr);
    }
  }
}
/*--------------- F U N C T I O N : M a s k ( ) ---------------*/
/*
PURPOSE
to return the right-justified mask value for given width 
of bits.

INPUT PARAMETERS
width of the mask

RETURN VALUE
the mask value
*/
CPU_INT32U Mask(CPU_INT08U width)
{
  assert(width>0&&width<=32);
  return ~(0xffffffff<<width);
}
/*--------------- F U N C T I O N : E x t r a c t ( ) ---------------*/
/*
PURPOSE
To extract the bits from the given value.

INPUT PARAMETERS
value - bits needed to be extracted from given value
lsb - position of least significant bit
masb - position of most significant bit

RETURN VALUE
extracted value
*/
CPU_INT32U Extract(CPU_INT32U value, CPU_INT08U lsb, CPU_INT08U msb)
{
  assert(lsb<32);
  assert(msb<32);
  assert(msb>=lsb);
  return (value>>lsb) & Mask(msb-lsb+1);
}

/*--------------- F U N C T I O N : I n s e r t ( ) ---------------*/
/*
PURPOSE
To insert the bits to the given value.

INPUT PARAMETERS
value - bits needed to be inserted to the given value
valInsert - value to be inserted
lsb - position of least significant bit
masb - position of most significant bit

RETURN VALUE
Value after inserting bits
*/
CPU_INT32U Insert(CPU_INT32U value, CPU_INT32U valInsert, CPU_INT08U lsb, CPU_INT08U msb)
{
  assert(lsb<32);
  assert(msb<32);
  assert(msb>=lsb);
  return (value & ~(Mask(msb-lsb+1)<<lsb)) | (valInsert<<lsb);
}
/*--------------- P u t R e p l y M s g ( ) ---------------

PURPOSE
Append a message to the reply buffer pair put buffer.

INPUT PARAMETERS
replyBfrPair  - the address of the Reply Buffer Pair
msg           - the address of the message string
*/
void  PutReplyMsg(CPU_INT08U *msg)
{
  while (*msg != '\0')
    PutByte(*msg++);
}
