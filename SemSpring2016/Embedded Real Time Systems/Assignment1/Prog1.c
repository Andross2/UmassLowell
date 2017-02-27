/*------------------- F I L E : p r o g 1 .c ----------------------*/

/*
by:	Naga Ganesh Kurapati
Embedded Real Time Systems
Electrical and Computer Engineering Dept.
UMASS Lowell

PURPOSE
Display the decoded messages from the PayLoad

CHANGES
01-30-2016 - Ported to STM32F107 Eval. Board, Part of Assignment-1
*/


/* Include Micrium and STM headers. */
#include "includes.h"

/* Include Paket Parser and Error Message Module headers*/
#include "PktParser.h"
#include "Error.h"
#include "assert.h"

/*----- c o n s t a n t    d e f i n i t i o n s -----*/
#define BaudRate 9600 
#define SpeedLength 2 /* wind speed array length */
#define DepthLength 2 /* precipitation depth array length */
#define DataLength 10 /* id array length */

#define PrecipitationLsb 1 /* precipitation message lsb index*/
#define PrecipitationMsb 0 /* precipitation message msb index*/
#define WindMsb 0 /* wind message msb index*/
#define WindLsb 1 /* wind message lsb index*/

/*----- f u n c t i o n    p r o t o t y p e s -----*/
int AppMain();
CPU_INT32U Mask(CPU_INT08U width);
CPU_INT32U Extract(CPU_INT32U value, CPU_INT08U lsb, CPU_INT08U msb);
CPU_INT32U Insert(CPU_INT32U value, CPU_INT32U valInsert, CPU_INT08U lsb, CPU_INT08U msb);
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
  CPU_INT08U    payloadLen ;
  CPU_INT08U    dstAddr ;
  CPU_INT08U    srcAddr ;
  CPU_INT08U    msgType ;
  union
  { 
    CPU_INT08S temp ;
    CPU_INT16U pres ;  
    struct
    {
      CPU_INT08S dewPt ;
      CPU_INT08U hum ;
    } hum ;
    struct
    {
      CPU_INT08U speed[SpeedLength] ;
      CPU_INT16U dir ;
    } wind ;
    CPU_INT16U rad ;
    CPU_INT32U dateTime ;
    CPU_INT08U depth[DepthLength] ;
    CPU_INT08U id[DataLength] ;
  } dataPart ; 
} Payload ;


/*--------------- m a i n ( ) -----------------*/

CPU_INT32S main()
{
  CPU_INT32S	exitCode;       // Return this code on exit.
  
  //  Initialize the STM32F107 eval. board.
  BSP_IntDisAll();            /* Disable all interrupts. */
  
  BSP_Init();                 /* Initialize BSP functions */
  
  BSP_Ser_Init(BaudRate);     /* Initialize the RS232 interface. */
  
  //  Run the application.  
  exitCode = AppMain();
  
  return exitCode;
}

/*--------------- F U N C T I O N : A p p M a i n ( ) ---------------*/

/*
PURPOSE
For given PayLoad, decodes the data to Display

INPUT PARAMETERS
no value

RETURN VALUE
no value
*/

CPU_INT32S AppMain()
{
  CPU_INT32U dateTimeMsg = 0; /* to read date and time message Lsb */
  Payload payLoadBfr; /* to read the payload */
  
  for(;;)
  {
    ParsePkt(&payLoadBfr); /* to parse the data recieved */
    if(payLoadBfr.dstAddr == DesinationAddress) /* executes the only if the desination address matches */
    {
      switch (payLoadBfr.msgType)
      {
        
        /* to diplay temperature message */
      case TemperatureMessage : 
        BSP_Ser_Printf("\nSOURCE NODE %d: TEMPERATURE MESSAGE\n \tTemperature = %d\n",payLoadBfr.srcAddr,payLoadBfr.dataPart.temp);
        break;
        
        /* to diplay pressure message */
      case PressureMessage : 
        BSP_Ser_Printf("\nSOURCE NODE %d: BAROMETRIC PRESSURE MESSAGE\n \tPressure = %d\n",payLoadBfr.srcAddr,
              Insert(Insert(payLoadBfr.dataPart.pres,Extract(payLoadBfr.dataPart.pres,8,15),0,7),Extract(payLoadBfr.dataPart.pres,0,7),8,15));
        break;
        
        /* to diplay dew point and humidity message */
      case HumidityMessage :
        BSP_Ser_Printf("\nSOURCE NODE %d: HUMIDITY MESSAGE\n \tDew Point = %d Humidity = %d\n",payLoadBfr.srcAddr,
                       payLoadBfr.dataPart.hum.dewPt,payLoadBfr.dataPart.hum.hum);
        break;
        
        /* to diplay wind speed and direction message */
      case WindMessage :
        BSP_Ser_Printf("\nSOURCE NODE %d: WIND MESSAGE\n \tSpeed = %d%d%d.%d Wind Direction = %d\n", payLoadBfr.srcAddr,
                       Extract(payLoadBfr.dataPart.wind.speed[WindMsb],4,7),
                       Extract(payLoadBfr.dataPart.wind.speed[WindMsb],0,3),
                       Extract(payLoadBfr.dataPart.wind.speed[WindLsb],4,7),
                       Extract(payLoadBfr.dataPart.wind.speed[WindLsb],0,3),
                       Insert(Insert(payLoadBfr.dataPart.wind.dir,Extract(payLoadBfr.dataPart.wind.dir,8,15),0,7),
                              Extract(payLoadBfr.dataPart.wind.dir,0,7),8,15));
        break;
        
        /* to diplay solar radiation message */
      case RadiationMessage :
        BSP_Ser_Printf("\nSOURCE NODE %d: SOLAR RADIATION MESSAGE\n \tSolar Radiation Intensity = %d\n",payLoadBfr.srcAddr,
                       Insert(Insert(payLoadBfr.dataPart.rad,Extract(payLoadBfr.dataPart.rad,8,15),0,7),
                              Extract(payLoadBfr.dataPart.rad,0,7),8,15));
        break;
        
        /* to diplay date and time message */
      case DateTimeMessage :
        dateTimeMsg = Insert(dateTimeMsg,Extract(payLoadBfr.dataPart.dateTime,24,31),0,7);
        dateTimeMsg = Insert(dateTimeMsg,Extract(payLoadBfr.dataPart.dateTime,16,23),8,15);
        dateTimeMsg = Insert(dateTimeMsg,Extract(payLoadBfr.dataPart.dateTime,8,15),16,23);
        dateTimeMsg = Insert(dateTimeMsg,Extract(payLoadBfr.dataPart.dateTime,0,7),24,31);
        payLoadBfr.dataPart.dateTime = dateTimeMsg;
        BSP_Ser_Printf("\nSOURCE NODE %d: DATE/TIME STAMP MESSAGE\n \tTime Stamp = %lu/%lu/%lu %lu:%lu\n", payLoadBfr.srcAddr,
                       Extract(payLoadBfr.dataPart.dateTime,5,8),
                       Extract(payLoadBfr.dataPart.dateTime,0,4),
                       Extract(payLoadBfr.dataPart.dateTime,9,20),
                       Extract(payLoadBfr.dataPart.dateTime,27,31),
                       Extract(payLoadBfr.dataPart.dateTime,21,26));
        break;
        
        /* to diplay precipitation depth message */
      case PrecipitationMessage :
        BSP_Ser_Printf("\nSOURCE NODE %d: PRECIPITATION MESSAGE\n \tPrecipitation Depth = %d%d.%d%d\n",payLoadBfr.srcAddr,
                       Extract(payLoadBfr.dataPart.depth[PrecipitationMsb],4,7),
                       Extract(payLoadBfr.dataPart.depth[PrecipitationMsb],0,3),
                       Extract(payLoadBfr.dataPart.depth[PrecipitationLsb],4,7),
                       Extract(payLoadBfr.dataPart.depth[PrecipitationLsb],0,3));
        break;
        
        /* to diplay sensor id message */
      case IdMessage :
        BSP_Ser_Printf("\nSOURCE NODE %d: SENSOR ID MESSAGE\n\tNode ID = %s",payLoadBfr.srcAddr,payLoadBfr.dataPart.id);
        break;
        
        /* to diplay unknown message error */
      default : 
        ErrorType(UnknownMessageError);
        break;
      }
    }
    else /* display if destination address doesn't match */
    {
      ErrorType(DestinationAddressError);
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

/*--------------- F U N C T I O N : E x t r a c t ( ) ---------------*/
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