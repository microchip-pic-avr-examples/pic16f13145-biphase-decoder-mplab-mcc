 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"

#define START_BYTE          0x0F    /* Start byte used for clock synch - first nibble must be 0*/
#define STOP_BYTE           0xF0    /* Stop byte, last nibble must be 0  */

#define BIPHASE_DECODER_BITRATE     (31250)
#define TIMER_PERIOD(x)        (uint8_t)((float)(_XTAL_FREQ / ( 8 * x)) - 1)

uint8_t FrameBuffer[64];
static uint8_t frameWrIndex = 0;
static uint8_t receivedByte;

inline void ResetFrame(void)
{
    frameWrIndex = 0; 
}

void ConfigureBitRate(uint32_t rate)
{
    TMR2_Stop();
    TMR2_Write(0x00);
    
    TMR2_PeriodCountSet(TIMER_PERIOD(rate));
    TMR2_Start();
}

/*
    Main application
*/

int main(void)
{
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts 
    // Use the following macros to: 

    // Enable the Global Interrupts 
    //INTERRUPT_GlobalInterruptEnable(); 

    // Disable the Global Interrupts 
    //INTERRUPT_GlobalInterruptDisable(); 

    // Enable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptEnable(); 

    // Disable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptDisable(); 
    
    printf("\n\r***************************************\n\r");
    printf("\n\r PIC16F13145 BiPhase Decoder Demo \n\r");
    printf("\n\r Configured bitrate %ld\n\r", (uint32_t)(BIPHASE_DECODER_BITRATE));
    printf("\n\r***************************************\n\r\n\r");
    
    //Set the TMR2 period to 8 * BIPHASE_DECODER_CLOCK
    ConfigureBitRate(BIPHASE_DECODER_BITRATE);

    SPI1_Open(CLIENT_CONFIG);
    
    while (1)
    {
        while (SPI1_IsRxReady() == true)
        {
            receivedByte = SPI1_ByteRead();
            FrameBuffer[frameWrIndex++] = receivedByte;
        }
        
        if (SS_SSP1_GetValue() == 1U)
        {        
            if (frameWrIndex > 0)
            {
                //Frame integrity check
                if ((FrameBuffer[0] == START_BYTE) && (FrameBuffer[frameWrIndex - 1] == STOP_BYTE))
                {
                    //Discard START and STOP bytes if the frame is valid
                    for (uint8_t i = 1; i < (frameWrIndex - 1); i++)
                    {
                        while(!(EUSART1_IsTxReady()))
                            ;
                        EUSART1_Write(FrameBuffer[i]);
                    }                    
                }
                ResetFrame();
            }
        }
    }
}