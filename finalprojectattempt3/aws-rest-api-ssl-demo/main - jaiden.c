//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
// Application Name     - SPI Demo
// Application Overview - The demo application focuses on showing the required
//                        initialization sequence to enable the CC3200 SPI
//                        module in full duplex 4-wire master and slave mode(s).
//
//*****************************************************************************


//*****************************************************************************
//
//! \addtogroup SPI_Demo
//! @{
//
//*****************************************************************************

// Standard includes
#include <string.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "uart.h"
#include "interrupt.h"


// Common interface includes
#include "uart_if.h"
#include "pin_mux_config.h"
#include "oled_test.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"
#include "glcdfont.h"
#include "i2c_if.h"
#include "gpio.h"
#include "gpio_if.h"


#define APPLICATION_VERSION     "1.4.0"
//*****************************************************************************
//
// Application Master/Slave mode selector macro
//
// MASTER_MODE = 1 : Application in master mode
// MASTER_MODE = 0 : Application in slave mode
//
//*****************************************************************************
#define MASTER_MODE      0

#define SPI_IF_BIT_RATE  100000
#define TR_BUFF_SIZE     100

#define MASTER_MSG       "This is CC3200 SPI Master Application\n\r"
#define SLAVE_MSG        "This is CC3200 SPI Slave Application\n\r"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
static unsigned char g_ucTxBuff[TR_BUFF_SIZE];
static unsigned char g_ucRxBuff[TR_BUFF_SIZE];
static unsigned char ucTxBuffNdx;
static unsigned char ucRxBuffNdx;

#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************



//*****************************************************************************
//
//! SPI Slave Interrupt handler
//!
//! This function is invoked when SPI slave has its receive register full or
//! transmit register empty.
//!
//! \return None.
//
//*****************************************************************************
static void SlaveIntHandler()
{
    unsigned long ulRecvData;
    unsigned long ulStatus;

    ulStatus = MAP_SPIIntStatus(GSPI_BASE,true);

    MAP_SPIIntClear(GSPI_BASE,SPI_INT_RX_FULL|SPI_INT_TX_EMPTY);

    if(ulStatus & SPI_INT_TX_EMPTY)
    {
        MAP_SPIDataPut(GSPI_BASE,g_ucTxBuff[ucTxBuffNdx%TR_BUFF_SIZE]);
        ucTxBuffNdx++;
    }

    if(ulStatus & SPI_INT_RX_FULL)
    {
        MAP_SPIDataGetNonBlocking(GSPI_BASE,&ulRecvData);
        g_ucTxBuff[ucRxBuffNdx%TR_BUFF_SIZE] = ulRecvData;
        Report("%c",ulRecvData);
        ucRxBuffNdx++;
    }
}

//*****************************************************************************
//
//! SPI Master mode main loop
//!
//! This function configures SPI modelue as master and enables the channel for
//! communication
//!
//! \return None.
//
//*****************************************************************************
void MasterMain()
{

    unsigned long ulUserData;
    unsigned long ulDummy;

    //
    // Initialize the message
    //
    memcpy(g_ucTxBuff,MASTER_MSG,sizeof(MASTER_MSG));

    //
    // Set Tx buffer index
    //
    ucTxBuffNdx = 0;
    ucRxBuffNdx = 0;

    //
    // Reset SPI
    //
    MAP_SPIReset(GSPI_BASE);

    //
    // Configure SPI interface
    //
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                           SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                           (SPI_SW_CTRL_CS |
                                   SPI_4PIN_MODE |
                                   SPI_TURBO_OFF |
                                   SPI_CS_ACTIVEHIGH |
                                   SPI_WL_8));

    //
    // Enable SPI for communication
    //
    MAP_SPIEnable(GSPI_BASE);

    //
    // Print mode on uart
    //
    Message("Enabled SPI Interface in Master Mode\n\r");

    //
    // User input
    //
    Report("Press any key to transmit data....");

    //
    // Read a character from UART terminal
    //
    ulUserData = MAP_UARTCharGet(UARTA0_BASE);


    //
    // Send the string to slave. Chip Select(CS) needs to be
    // asserted at start of transfer and deasserted at the end.
    //
    MAP_SPITransfer(GSPI_BASE,g_ucTxBuff,g_ucRxBuff,50,
                    SPI_CS_ENABLE|SPI_CS_DISABLE);

    //
    // Report to the user
    //
    Report("\n\rSend      %s",g_ucTxBuff);
    Report("Received  %s",g_ucRxBuff);

    //
    // Print a message
    //
    Report("\n\rType here (Press enter to exit) :");

    //
    // Initialize variable
    //
    ulUserData = 0;

    //
    // Enable Chip select
    //
    MAP_SPICSEnable(GSPI_BASE);

    //
    // Loop until user "Enter Key" is
    // pressed
    //
    while(ulUserData != '\r')
    {
        //
        // Read a character from UART terminal
        //
        ulUserData = MAP_UARTCharGet(UARTA0_BASE);

        //
        // Echo it back
        //
        MAP_UARTCharPut(UARTA0_BASE,ulUserData);

        //
        // Push the character over SPI
        //
        MAP_SPIDataPut(GSPI_BASE,ulUserData);

        //
        // Clean up the receive register into a dummy
        // variable
        //
        MAP_SPIDataGet(GSPI_BASE,&ulDummy);
    }

    //
    // Disable chip select
    //
    MAP_SPICSDisable(GSPI_BASE);
}

//*****************************************************************************
//
//! SPI Slave mode main loop
//!
//! This function configures SPI modelue as slave and enables the channel for
//! communication
//!
//! \return None.
//
//*****************************************************************************
void SlaveMain()
{
    //
    // Initialize the message
    //
    memcpy(g_ucTxBuff,SLAVE_MSG,sizeof(SLAVE_MSG));

    //
    // Set Tx buffer index
    //
    ucTxBuffNdx = 0;
    ucRxBuffNdx = 0;

    //
    // Reset SPI
    //
    MAP_SPIReset(GSPI_BASE);

    //
    // Configure SPI interface
    //
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                           SPI_IF_BIT_RATE,SPI_MODE_SLAVE,SPI_SUB_MODE_0,
                           (SPI_HW_CTRL_CS |
                                   SPI_4PIN_MODE |
                                   SPI_TURBO_OFF |
                                   SPI_CS_ACTIVEHIGH |
                                   SPI_WL_8));

    //
    // Register Interrupt Handler
    //
    MAP_SPIIntRegister(GSPI_BASE,SlaveIntHandler);

    //
    // Enable Interrupts
    //
    MAP_SPIIntEnable(GSPI_BASE,SPI_INT_RX_FULL|SPI_INT_TX_EMPTY);

    //
    // Enable SPI for communication
    //
    MAP_SPIEnable(GSPI_BASE);

    //
    // Print mode on uart
    //
    Message("Enabled SPI Interface in Slave Mode\n\rReceived : ");
}

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
    /* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//! Main function for spi demo application
//!
//! \param none
//!
//! \return None.
//
//*****************************************************************************

int calculateDelta(signed char readValue){
    //These numbers are arbitrary. Was just trial and error to fine tune these numbers
    //This will effectively just make values that are less intense have a lesser true delta
    //Values that are more intense/higher will yield a higher delta. Most of the work is done
    //by normalizing max
    //Takes about 2 seconds to cross the screen
    float sensitivity = 8.5;
    float normalizingMax = 45;
    float normalizedDelta = ((float)readValue) / normalizingMax;
    float trueDelta = normalizedDelta * sensitivity;
    return (int) trueDelta;
}

void drawBall(unsigned char x, unsigned char y){
    //Whatever color you like for the ball
    fillCircle(x, y, BALLRADIUS, GREEN);
}

void eraseBall(unsigned char x, unsigned char y){
    //Erasing is effectively drawing a black ball at some location
    //Filling the screen black in between would be far too slow
    fillCircle(x,y, BALLRADIUS, BLACK);
}

//Variables to use later. Size of 256 was selected because that's the same size as buffer shown in i2c_demo
unsigned char dvcAdr = 0x18;
unsigned char dvcRegOffSetX = 0x3;
unsigned char dvcRegOffSetY = 0x5;
unsigned char rdDataBufX[256];
unsigned char rdDataBufY[256];

//Ball should start at center
int x = WIDTH/2;
int y = HEIGHT/2;

int goalX = WIDTH/2;
int goalY = HEIGHT - 10;

int scoreCounter = 0;
int goalSwitch = 0;

void makeBall() {
    if(goalSwitch == 0) {
        drawGoalBottom(goalX, goalY, 10, 10, BLUE);
    }
    else if(goalSwitch == 2) {
        drawGoalLeft(goalX, goalY, 10, 10, BLUE);
    }
    else if(goalSwitch == 1) {
        drawGoalTop(goalX, goalY, 10, 10, BLUE);
    }
    else {
        drawGoalRight(goalX, goalY, 10, 10, BLUE);
    }

    drawBall(x,y);

    //Save to erase later
    unsigned char oldX = x;
    unsigned char oldY = y;

    //Both are the same exact process/lines of code from the i2c_demo file for reading from a register.
    //Reading from the x offset register
    I2C_IF_Write(dvcAdr,&dvcRegOffSetX,1,0);
    I2C_IF_Read(dvcAdr, &rdDataBufX[0], 1);
    //Reading from the y offset register
    I2C_IF_Write(dvcAdr,&dvcRegOffSetY,1,0);
    I2C_IF_Read(dvcAdr, &rdDataBufY[0], 1);
    int dx = calculateDelta(rdDataBufX[0]);
    int dy = calculateDelta(rdDataBufY[0]);


    //Prevent movement out of the screen and prevents the ball from partially going off screen
    x = x + dx;
    if(x > WIDTH - BALLRADIUS) {
        x = WIDTH - BALLRADIUS;
        scoreCounter++;
        x = WIDTH/2;
        y = HEIGHT/2;
    }
    if(x < BALLRADIUS) {
        x = BALLRADIUS;
        scoreCounter++;
        x = WIDTH/2;
        y = HEIGHT/2;
    }

    y = y + dy;
    if(y > HEIGHT - BALLRADIUS) {
        y = HEIGHT - BALLRADIUS;
        scoreCounter++;
        x = WIDTH/2;
        y = HEIGHT/2;
    }
    if(y < BALLRADIUS) {
        y = BALLRADIUS;
        scoreCounter++;
        x = WIDTH/2;
        y = HEIGHT/2;
    }

    //Erasing the ball at the old location
    eraseBall(oldX, oldY);

    if((x < (goalX + 8) && x > (goalX - 8)) && (y < (goalY + 8) && y > (goalY - 8))) {
        Message("\n\rYay!\n\r");
        x = WIDTH/2;
        y = HEIGHT/2;
        scoreCounter++;
        if(goalSwitch == 0) { //Removes bottom goal, makes top goal
            drawGoalBottom(goalX, goalY, 10, 10, BLACK);
            goalSwitch = 1;
            goalY = 10;
            goalX = WIDTH/2;
        }
        else if (goalSwitch == 1) { //Removes top goal, makes right goal
            drawGoalTop(goalX, goalY, 10, 10, BLACK);
            goalSwitch = 3;
            goalX = WIDTH - 10;
            goalY = HEIGHT/2;
        }
        else if(goalSwitch == 3) { //Removes right goal, makes left goal
            drawGoalRight(goalX, goalY, 10, 10, BLACK);
            goalSwitch = 2;
            goalX = 10;
            goalY = HEIGHT/2;

        }
        else { //Removes left goal, makes bottom goal
            drawGoalLeft(goalX, goalY, 10, 10, BLACK);
            goalSwitch = 0;
            goalY = HEIGHT - 10;
            goalX = WIDTH/2;
        }

        if(scoreCounter == 15) {
            Message("\n\rYour Winner!\n\r");
        }
    }

}


void main()
{
    //
    // Initialize Board configurations
    //
    BoardInit();

    //
    // Muxing UART and SPI lines.
    //
    PinMuxConfig();

    I2C_IF_Open(I2C_MASTER_MODE_FST);

    //
    // Enable the SPI module clock
    //
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

    //
    // Initialising the Terminal.
    //
    InitTerm();

    //
    // Clearing the Terminal.
    //
    ClearTerm();

    //
    // Reset the peripheral
    //
    MAP_PRCMPeripheralReset(PRCM_GSPI);

    //
    // Reset SPI
    //
    MAP_SPIReset(GSPI_BASE);

    //
    // Configure SPI interface
    //
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                           SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                           (SPI_SW_CTRL_CS |
                                   SPI_4PIN_MODE |
                                   SPI_TURBO_OFF |
                                   SPI_CS_ACTIVEHIGH |
                                   SPI_WL_8));

    //
    // Enable SPI for communication
    //
    MAP_SPIEnable(GSPI_BASE);

    //Call this after when the proper prior functions are called (in the correct order too)
    Adafruit_Init();

    //Empty the screen first
    fillScreen(BLACK);

    while(1)
    {
        makeBall();
    }
}
