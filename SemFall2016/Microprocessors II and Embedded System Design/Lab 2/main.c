/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB(c) Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 3.16
        Device            :  PIC16F18857
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.20
 */

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */

#include "mcc_generated_files/mcc.h"


/*
 Main application
 */

uint8_t read(void)
{
    uint8_t data;
    IO_RC0_SetDigitalInput();
    IO_RC1_SetDigitalInput();
    IO_RC2_SetDigitalInput();
    IO_RC3_SetDigitalInput();
    while(!IO_RB2_GetValue());
    while(IO_RB2_GetValue())
    data = PORTC;
    return (data & (0x0F));
}

void write(uint8_t data)
{
    IO_RC0_SetDigitalOutput();
    IO_RC1_SetDigitalOutput();
    IO_RC2_SetDigitalOutput();
    IO_RC3_SetDigitalOutput();
    while(!IO_RB2_GetValue());
    while(IO_RB2_GetValue())
    PORTC = data;
    PORTC = 0x00;
}

void main(void) 
{
    SYSTEM_Initialize();
    while (1) 
    {
        uint8_t readValue = read();
        if(readValue==0x00)
        {
            write(0x0E);
            SYSTEM_Initialize();
        }
        else if(readValue==0x01)
        {
            write(0x0E);
        }
        else if(readValue==0x02)
        {
            uint16_t adc_result = ADCC_GetSingleConversion(channel_ANA2);
            uint16_t nib1=0, nib2=0, nib3=0;
            nib1 = adc_result & 0x0f;
            nib2 = (adc_result >> 4) & 0x0f;
            nib3 = (adc_result >> 8) & 0x03;
            write(nib1);
            write(nib2);
            write(nib3);
            write(0x0E);
        }
    }
}
/**
 End of File
 */