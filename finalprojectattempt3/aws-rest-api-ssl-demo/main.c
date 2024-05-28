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
// Application Name     -   SSL Demo
// Application Overview -   This is a sample application demonstrating the
//                          use of secure sockets on a CC3200 device.The
//                          application connects to an AP and
//                          tries to establish a secure connection to the
//                          Google server.
// Application Details  -
// docs\examples\CC32xx_SSL_Demo_Application.pdf
// or
// http://processors.wiki.ti.com/index.php/CC32xx_SSL_Demo_Application
//
//*****************************************************************************


//*****************************************************************************
//
//! \addtogroup ssl
//! @{
//
//*****************************************************************************

#include <stdio.h>
#include <stdlib.h>  // for rand, srand
#include <time.h>    // for time
// Simplelink includes
#include "simplelink.h"

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"

//Common interface includes
#include "pin_mux_config.h"
#include "gpio_if.h"
#include "common.h"
#include "uart_if.h"
#include "gpio_if.h"
#include "gpio_if.c"
#include "network_common.c"
#include "startup_ccs.c"

// Custom includes
#include "utils/network_utils.h"

// Standard includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Driverlib includes
#include "hw_types.h" //
#include "hw_ints.h"//
#include "hw_nvic.h"
#include "hw_memmap.h"//
#include "hw_common_reg.h"
#include "interrupt.h" //
#include "hw_apps_rcm.h"
#include "prcm.h"
#include "rom.h"
#include "prcm.h"
#include "utils.h"
#include "systick.h"
#include "rom_map.h"
#include "gpio.h"
#include "spi.h"
#include "uart.h"


// Common interface includes
#include "uart_if.h"
#include "oled_test.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"
#include "glcdfont.h"
// Pin configurations
#include "pin_mux_config.h"
#include "i2c_if.h"


//Lab 3 Macros
#define SPI_IF_BIT_RATE  100000
#define IR_GPIO_PORT GPIOA0_BASE
#define IR_GPIO_PIN 0x1

#define EXPECTEDFALLINGEDGES   36
#define SPI_IF_BIT_RATE  100000

volatile int buttonpresses = 0;
volatile char currentCharacter = '\0';

#define BUFFER_SIZE 100
char bufferToSend[BUFFER_SIZE];
int bufferIndex = 0;
int sendBufferHeight = 16;
int displayToSendMessageIndex = 6;



volatile int decodingIndex;
volatile int systickexpired;
volatile uint64_t ulsystick_delta_us;

volatile int currentButton[EXPECTEDFALLINGEDGES];
int zero[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,0,0,0,0,1,0,0,0,1,1,1,1,0,1,1,1,1,1};
int one[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,0,0,0,1,0,0,0,0,1,1,1,0,1,1,1,1,1};
int two[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,0,0,1,0,0,0,1,0,1,1,0,1,1,1,1,1};
int three[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1};
int four[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,0,0,1,0,1,0,0,0,1,1,0,1,0,1,1,1,1,1};
int five[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,0,1,0,1,0,0,0,0,1,0,1,0,1,1,1,1,1};
int six[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,1,0,1,0,0,0,1,0,0,1,0,1,1,1,1,1};
int seven[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,1,0,1,1,1,1,1};
int eight[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1};
int nine[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,1,1,1};
int last[] = {1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,0,1,1,0,0,0,1,0,1,0,0,1,1,1,1,1};
int mute[] ={1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,1,0,1,0,1,0,0,0,0,1,0,1,0,1,1,1,1,1,1};



// the cc3200's fixed clock frequency of 80 MHz
// note the use of ULL to indicate an unsigned long long constant
#define SYSCLKFREQ 80000000ULL

// macro to convert ticks to microseconds
#define TICKS_TO_US(ticks) \
        ((((ticks) / SYSCLKFREQ) * 1000000ULL) + \
                ((((ticks) % SYSCLKFREQ) * 1000000ULL) / SYSCLKFREQ))\

// macro to convert microseconds to ticks
#define US_TO_TICKS(us) ((SYSCLKFREQ / 1000000ULL) * (us))

// systick reload value set to 16ms period
// (PERIOD_SEC) * (SYSCLKFREQ) = PERIOD_TICKS
#define SYSTICK_RELOAD_VAL 12800000UL


extern void (* const g_pfnVectors[])(void);



//Lab 4 Macros
#define DATE                17    /* Current Date */
#define MONTH               5     /* Month 1-12 */
#define YEAR                2024  /* Current year */
#define HOUR                11   /* Time - hours */
#define MINUTE              2   /* Time - minutes */
#define SECOND              0     /* Time - seconds */


#define APPLICATION_NAME      "SSL"
#define APPLICATION_VERSION   "SQ24"
#define SERVER_NAME           "a1yrxw8th6s3sr-ats.iot.us-east-1.amazonaws.com" // CHANGE ME
#define GOOGLE_DST_PORT       8443

#define GETHEADER "GET /things/Raph_CC3200_Thing/shadow HTTP/1.1\r\n"
#define POSTHEADER "POST /things/Raph_CC3200_Thing/shadow HTTP/1.1\r\n"             // CHANGE ME
#define HOSTHEADER "Host: a1yrxw8th6s3sr-ats.iot.us-east-1.amazonaws.com\r\n"  // CHANGE ME
#define CHEADER "Connection: Keep-Alive\r\n"
#define CTHEADER "Content-Type: application/json; charset=utf-8\r\n"
#define CLHEADER1 "Content-Length: "
#define CLHEADER2 "\r\n\r\n"

#define DATA1 "{" \
        "\"state\": {\r\n"                                              \
            "\"desired\" : {\r\n"                                       \
                "\"var\" :\""                                           \
                "Hello phone, "                                     \
                "message from CC3200 via AWS IoT!"                  \
                "\"\r\n"                                            \
                "}"                                                         \
                "}"                                                             \
                "}\r\n\r\n"


//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************

#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

//*****************************************************************************
//                 GLOBAL VARIABLES -- End: df
//*****************************************************************************


//****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//****************************************************************************
static int set_time();
static void BoardInit(void);
static int http_post(int);
static int http_get(int);
static int postCustomEmail(int);

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************

static inline void SysTickReset(void) {
    // any write to the ST_CURRENT register clears it
    // after clearing it automatically gets reset without
    // triggering exception logic
    // see reference manual section 3.2.1
    HWREG(NVIC_ST_CURRENT) = 1;
    systickexpired = 0;

}

static void SysTickHandler(void) {
    systickexpired = 1;
    if (decodingIndex != 0){
        decodingIndex = 0;
    }

}


static void BoardInit(void) {
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

static void SysTickInit(void) {

    // configure the reset value for the systick countdown register
    MAP_SysTickPeriodSet(SYSTICK_RELOAD_VAL);

    // register interrupts on the systick module
    MAP_SysTickIntRegister(SysTickHandler);

    // enable interrupts on systick
    // (trigger SysTickHandler when countdown reaches 0)
    MAP_SysTickIntEnable();

    // enable the systick module itself
    MAP_SysTickEnable();

}



//*****************************************************************************
//
//! This function updates the date and time of CC3200.
//!
//! \param None
//!
//! \return
//!     0 for success, negative otherwise
//!
//*****************************************************************************

static int set_time() {
    long retVal;

    g_time.tm_day = DATE;
    g_time.tm_mon = MONTH;
    g_time.tm_year = YEAR;
    g_time.tm_sec = HOUR;
    g_time.tm_hour = MINUTE;
    g_time.tm_min = SECOND;

    retVal = sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION,
                       SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME,
                       sizeof(SlDateTime),(unsigned char *)(&g_time));

    ASSERT_ON_ERROR(retVal);
    return SUCCESS;
}

//*****************************************************************************
//
//! Main
//!
//! \param  none
//!
//! \return None
//!
//*****************************************************************************

void displayToSendMessageChar(char c){

    if (0 + (displayToSendMessageIndex * 6) >= WIDTH - 10){
        displayToSendMessageIndex = 0;
        sendBufferHeight += 12;
    }

    drawChar(0 + (displayToSendMessageIndex * 6), sendBufferHeight, c, WHITE, BLACK, 1);
}


int arraysEqual(volatile int arr1[], int arr2[], int size) {
    int i = 0;
    for (i = 0; i < size; i++) {
        if (arr1[i] != arr2[i]) {
            return 0;
        }
    }
    return 1;
}

const char* addTextToJSON(const char* inputText) {
    const char* templateJSON = "{" \
            "\"state\": {\r\n" \
            "\"desired\" : {\r\n" \
            "\"var\" :\"" \
            "%s" \
            "\"\r\n" \
            "}" \
            "}"                                                             \
            "}\r\n\r\n";




    // Calculate the required length for the new JSON string
    size_t templateLen = strlen(templateJSON);
    size_t inputLen = strlen(inputText);
    size_t newJSONLen = templateLen + inputLen + 1; // +1 for null terminator

    // Allocate memory for the new JSON string
    char* newJSON = (char*)malloc(newJSONLen);
    if (newJSON == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    // Format the new JSON string
    snprintf(newJSON, newJSONLen, templateJSON, inputText);

    return newJSON;
}


static int postCustomEmail(int iTLSSockID){
    char acSendBuff[512];
    char acRecvbuff[1460];
    char cCLLength[200];
    char* pcBufHeaders;
    int lRetVal = 0;

    const char* JSONtoSend = addTextToJSON(bufferToSend);

    //UART_PRINT("\r\n\nResulting JSON: \r\n\n");
    //UART_PRINT(JSONtoSend);

    pcBufHeaders = acSendBuff;
    strcpy(pcBufHeaders, POSTHEADER);
    pcBufHeaders += strlen(POSTHEADER);
    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);
    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);
    strcpy(pcBufHeaders, "\r\n\r\n");

    int dataLength = strlen(JSONtoSend);

    strcpy(pcBufHeaders, CTHEADER);
    pcBufHeaders += strlen(CTHEADER);
    strcpy(pcBufHeaders, CLHEADER1);

    pcBufHeaders += strlen(CLHEADER1);
    sprintf(cCLLength, "%d", dataLength);

    strcpy(pcBufHeaders, cCLLength);
    pcBufHeaders += strlen(cCLLength);
    strcpy(pcBufHeaders, CLHEADER2);
    pcBufHeaders += strlen(CLHEADER2);

    strcpy(pcBufHeaders, JSONtoSend);
    pcBufHeaders += strlen(JSONtoSend);

    int testDataLength = strlen(pcBufHeaders);

    UART_PRINT(acSendBuff);


    //
    // Send the packet to the server */
    //
    lRetVal = sl_Send(iTLSSockID, acSendBuff, strlen(acSendBuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("POST failed. Error Number: %i\n\r",lRetVal);
        sl_Close(iTLSSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }
    lRetVal = sl_Recv(iTLSSockID, &acRecvbuff[0], sizeof(acRecvbuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("Received failed. Error Number: %i\n\r",lRetVal);
        //sl_Close(iSSLSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }
    else {
        acRecvbuff[lRetVal+1] = '\0';
        UART_PRINT(acRecvbuff);
        UART_PRINT("\n\r\n\r");
    }

    return 0;
}


void sendEmail(){
    Report("\n\n\rSending email!\n\n\r");

    //Report("\n\r This is your email: ");
    //Report(bufferToSend);
    long lRetVal = -1;

    // initialize global default app configuration
    g_app_config.host = SERVER_NAME;
    g_app_config.port = GOOGLE_DST_PORT;

    //Connect the CC3200 to the local access point
    lRetVal = connectToAccessPoint();
    //Set time so that encryption can be used
    lRetVal = set_time();
    if(lRetVal < 0) {
        UART_PRINT("Unable to set time in the device");
        LOOP_FOREVER();
    }
    //Connect to the website with TLS encryption
    lRetVal = tls_connect();
    if(lRetVal < 0) {
        ERR_PRINT(lRetVal);
    }
    postCustomEmail(lRetVal);

    sl_Stop(SL_STOP_TIMEOUT);
    LOOP_FOREVER();
}


void addToBufferChar(char c) {
    if (bufferIndex < BUFFER_SIZE - 1) {
        bufferToSend[bufferIndex++] = c;
        bufferToSend[bufferIndex] = '\0';
    }
}

static void addToBuf(){
    addToBufferChar(currentCharacter);
    displayToSendMessageChar(currentCharacter);
    displayToSendMessageIndex++;
    currentCharacter = '\0';
}


static void leaderBoardTyping() {

    if (arraysEqual(currentButton, one, EXPECTEDFALLINGEDGES)){
        //Report("One\n\r");
    }
    else if (arraysEqual(currentButton, two, EXPECTEDFALLINGEDGES)){
        //Report("Two\n\r");
        if (buttonpresses % 3 == 0){
            displayToSendMessageChar('a');
            currentCharacter = 'a';
        }
        else if (buttonpresses % 3 == 1){
            //displayToSendMessageChar('b');
            displayToSendMessageChar('b');
            currentCharacter = 'b';
        } else if (buttonpresses % 3 == 2){
            //displayToSendMessageChar('c');
            displayToSendMessageChar('c');
            currentCharacter = 'c';
        }
        buttonpresses = buttonpresses + 1;

    } else if (arraysEqual(currentButton,three, EXPECTEDFALLINGEDGES)){
        //Report("Three\n\r");
        if (buttonpresses % 3 == 0){
            displayToSendMessageChar('d');
            currentCharacter = 'd';
        }
        else if (buttonpresses % 3 == 1){
            displayToSendMessageChar('e');
            currentCharacter = 'e';
        } else if (buttonpresses % 3 == 2){
            displayToSendMessageChar('f');
            currentCharacter = 'f';
        }
        buttonpresses = buttonpresses + 1;
    } else if (arraysEqual(currentButton,four, EXPECTEDFALLINGEDGES)){
        //Report("Four\n\r");
        if (buttonpresses % 3 == 0){
            displayToSendMessageChar('g');
            currentCharacter = 'g';
        }
        else if (buttonpresses % 3 == 1){
            displayToSendMessageChar('h');
            currentCharacter = 'h';
        } else if (buttonpresses % 3 == 2){
            displayToSendMessageChar('i');
            currentCharacter = 'i';
        }
        buttonpresses = buttonpresses + 1;
    } else if (arraysEqual(currentButton,five, EXPECTEDFALLINGEDGES)){
        //Report("Five\n\r");
        if (buttonpresses % 3 == 0){
            displayToSendMessageChar('j');
            currentCharacter = 'j';
        }

        else if (buttonpresses % 3 == 1){
            displayToSendMessageChar('k');
            currentCharacter = 'k';
        } else if (buttonpresses % 3 == 2){
            displayToSendMessageChar('l');
            currentCharacter = 'l';
        }
        buttonpresses = buttonpresses + 1;
    } else if (arraysEqual(currentButton,six, EXPECTEDFALLINGEDGES)){
        //Report("Six\n\r");
        if (buttonpresses % 3 == 0){
            displayToSendMessageChar('m');
            currentCharacter = 'm';
        }
        else if (buttonpresses % 3 == 1){
            displayToSendMessageChar('n');
            currentCharacter = 'n';
        } else if (buttonpresses % 3 == 2){
            displayToSendMessageChar('o');
            currentCharacter = 'o';
        }
        buttonpresses = buttonpresses + 1;
    } else if (arraysEqual(currentButton,seven, EXPECTEDFALLINGEDGES)){
        //Report("Seven\n\r");
        if (buttonpresses % 4 == 0){
            displayToSendMessageChar('p');
            currentCharacter = 'p';
        }
        else if (buttonpresses % 4 == 1){
            displayToSendMessageChar('q');
            currentCharacter = 'q';
        } else if (buttonpresses % 4 == 2){
            displayToSendMessageChar('r');
            currentCharacter = 'r';
        } else if (buttonpresses % 4 == 3){
            displayToSendMessageChar('s');
            currentCharacter = 's';
        }
        buttonpresses = buttonpresses + 1;
    } else if (arraysEqual(currentButton,eight, EXPECTEDFALLINGEDGES)){
        //Report("Eight\n\r");
        if (buttonpresses % 3 == 0){
            displayToSendMessageChar('t');
            currentCharacter = 't';
        }
        else if (buttonpresses % 3 == 1){
            displayToSendMessageChar('u');
            currentCharacter = 'u';
        } else if (buttonpresses % 3 == 2){
            displayToSendMessageChar('v');
            currentCharacter = 'v';
        }
        buttonpresses = buttonpresses + 1;
    } else if (arraysEqual(currentButton,nine, EXPECTEDFALLINGEDGES)){
        //Report("Nine\n\r");
        if (buttonpresses % 4 == 0){
            displayToSendMessageChar('w');
            currentCharacter = 'w';
        }
        else if (buttonpresses % 4 == 1){
            displayToSendMessageChar('x');
            currentCharacter = 'x';
        } else if (buttonpresses % 4 == 2){
            displayToSendMessageChar('y');
            currentCharacter = 'y';
        } else if (buttonpresses % 4 == 3){
            displayToSendMessageChar('z');
            currentCharacter = 'z';
        }
        buttonpresses = buttonpresses + 1;
    } else if (arraysEqual(currentButton,last, EXPECTEDFALLINGEDGES) && currentCharacter == '\0'){
        //Backspace
        if (bufferIndex != 0){
            displayToSendMessageIndex--;
            displayToSendMessageChar(' ');
            //Pop the top member and decrement buffer index
            bufferToSend[bufferIndex - 1] = '\0';
            bufferIndex--;
        }

    } else if (arraysEqual(currentButton,mute, EXPECTEDFALLINGEDGES)){
        //Report("Mute\n\r");
        sendEmail();
        bufferIndex = 0;



    }else if (arraysEqual(currentButton,zero, EXPECTEDFALLINGEDGES)){
        //Report("Zero\n\r");
        currentCharacter = ' ';
    }
    else{
        //Report("Unrecognized Key\n\r");
        currentCharacter = '\0';
        buttonpresses = 0;
    }

    //Flush current button too to be safe
    int i = 0;
    for (i = 0; i < EXPECTEDFALLINGEDGES; i++) {
        //-1 is neither a short or falling edge
        currentButton[i] = -1;
    }

    SysTickReset();
    SysTickEnable();
    GPIOIntEnable(IR_GPIO_PORT, IR_GPIO_PIN);

}


static int evaluateCapturedPulses() {

    SysTickReset();
    SysTickEnable();
    GPIOIntEnable(IR_GPIO_PORT, IR_GPIO_PIN);
    if (arraysEqual(currentButton, one, EXPECTEDFALLINGEDGES)){
        return 1;
    }
    else if (arraysEqual(currentButton, two, EXPECTEDFALLINGEDGES)){
        return 2;
    } else if (arraysEqual(currentButton,three, EXPECTEDFALLINGEDGES)){
        return 3;
    } else if (arraysEqual(currentButton,four, EXPECTEDFALLINGEDGES)){
        return 4;
    } else if (arraysEqual(currentButton,five, EXPECTEDFALLINGEDGES)){
        return 5;
    } else if (arraysEqual(currentButton,six, EXPECTEDFALLINGEDGES)){
        return 6;
    } else if (arraysEqual(currentButton,seven, EXPECTEDFALLINGEDGES)){
        return 7;
    } else if (arraysEqual(currentButton,eight, EXPECTEDFALLINGEDGES)){
        return 8;
    } else if (arraysEqual(currentButton,nine, EXPECTEDFALLINGEDGES)){
        return 9;
    } else if (arraysEqual(currentButton,last, EXPECTEDFALLINGEDGES) && currentCharacter == '\0'){

        return 10;

    } else if (arraysEqual(currentButton,mute, EXPECTEDFALLINGEDGES)){
        return 11;
    }else if (arraysEqual(currentButton,zero, EXPECTEDFALLINGEDGES)){
        return 0;
    }
    else{
        return -1;
        currentCharacter = '\0';
        buttonpresses = 0;
    }

    //Flush current button too to be safe
    int i = 0;
    for (i = 0; i < EXPECTEDFALLINGEDGES; i++) {
        //-1 is neither a short or falling edge
        currentButton[i] = -1;
    }

    return -1;
}

static void decodeSignal(uint64_t tmp_delta_us, int count) {
    //1s are encoded as a long amount of time from last falling edge
    //0s are encoded as a short amount of time from last falling edge

    if(tmp_delta_us > 1200) {

        currentButton[count] = 1;
    }
    else{
        currentButton[count] = 0;
    }
}

static void ButtonSignalHandler(void){

    unsigned long ulStatus;
    ulStatus = MAP_GPIOIntStatus(IR_GPIO_PORT, true);
    MAP_GPIOIntClear(IR_GPIO_PORT, ulStatus);
    uint64_t tmp_delta_us = ulsystick_delta_us;

    if (ulStatus && IR_GPIO_PIN){
        tmp_delta_us = TICKS_TO_US(SYSTICK_RELOAD_VAL - MAP_SysTickValueGet());

        if (tmp_delta_us > 600){ //Filter out some noise
            decodeSignal(tmp_delta_us, decodingIndex);
            decodingIndex++;
            SysTickReset();
        }
    }
    return;

}



void SpiConfig(){
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

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
}

void HandlerConfig(){
    MAP_GPIOIntRegister(IR_GPIO_PORT, ButtonSignalHandler);
    MAP_GPIOIntTypeSet(IR_GPIO_PORT, IR_GPIO_PIN, GPIO_FALLING_EDGE);
    uint64_t ulStatus = MAP_GPIOIntStatus(IR_GPIO_PORT, false);
    MAP_GPIOIntClear(IR_GPIO_PORT, ulStatus);
    MAP_GPIOIntEnable(IR_GPIO_PORT,IR_GPIO_PIN);
}

void seedRandomGenerator() {
    // Seed the random number generator with the current time
    srand((unsigned int)time(0));
}

int playerScore;

void updateScore(){
    setTextColorTransparent(WHITE);
    char scoreStr[50];
    sprintf(scoreStr, "Score: %i", playerScore);
    fillRect(19,HEIGHT - 9, WIDTH, 9, BLACK);
    setCursor(20,HEIGHT - 8);
    Outstr(scoreStr);
}

int generateRandomNumber(int lower, int upper) {
    // Ensure the range is valid
    if (lower > upper) {
        return 1;
    }

    // Generate a random number in the range [lower, upper]
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}


//For accelerometer
unsigned char dvcAdr = 0x18;
unsigned char dvcRegOffSetX = 0x3;
unsigned char dvcRegOffSetY = 0x5;
unsigned char rdDataBufX[256];
unsigned char rdDataBufY[256];


int calculateDelta(signed char readValue){
    float sensitivity = 12;
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



void accelCarMinigame(){
    fillScreen(BLACK);
    setCursor(19, 0);
    Outstr("Dodge the Cars!");
    updateScore();

    int ballX = WIDTH/4;
    int ballY = HEIGHT/2;
    int carWidth = 30;
    int carHeight = 15;

    int i = 0;
    int gameDone = 0;
    while (!gameDone){
        //15 cars
        for (i = 0; i < 15 && !gameDone; i++){
            int j = 0;
            int randomCarYSpawn = generateRandomNumber(12,HEIGHT - 12 - carHeight);

            //This equates to 7 frames of car movement
            for (j = 0; j < WIDTH/15 && !gameDone; j++){
                fillRect(WIDTH - 30 - (j * 15),randomCarYSpawn, carWidth, carHeight, RED);

                if (j == 5 || j == 6 || j == 7){ //Ball can touch car on these frames
                    if (randomCarYSpawn + carHeight > ballY && ballY >= randomCarYSpawn){
                        gameDone = 1;
                    }
                }


                drawBall(ballX,ballY);
                unsigned char oldY = ballY;
                I2C_IF_Write(dvcAdr,&dvcRegOffSetY,1,0);
                I2C_IF_Read(dvcAdr, &rdDataBufY[0], 1);
                int dy = calculateDelta(rdDataBufY[0]);

                ballY = ballY + dy;
                if(ballY > HEIGHT - 12 - BALLRADIUS)
                    ballY = HEIGHT - 12 - BALLRADIUS;
                if(ballY < BALLRADIUS + 12)
                    ballY = BALLRADIUS + 12;

                //Erasing
                eraseBall(ballX, oldY);
                fillRect(WIDTH - 30 - (j * 15),randomCarYSpawn, carWidth, carHeight, BLACK);
            }


            playerScore += 100;
            updateScore();
        }
        gameDone = 1;
    }

    fillScreen(BLACK);

}

void passwordMinigame(){
    fillScreen(BLACK);
    setCursor(19, 0);
    Outstr("Guess the code");
    updateScore();
    int firstDigitAnswer = generateRandomNumber(1,9);
    int secondDigitAnswer = generateRandomNumber(1,9);
    int thirdDigitAnswer = generateRandomNumber(1,9);
    int trueAnswer[3] = {firstDigitAnswer,secondDigitAnswer,thirdDigitAnswer};
    int userAnswer[3] = {1,1,1};
    int userIndex = 0;

    drawChar(19,HEIGHT/2-8,'1',WHITE,BLACK,2);
    drawChar(59,HEIGHT/2-8,'1',WHITE,BLACK,2);
    drawChar(99,HEIGHT/2-8,'1',WHITE,BLACK,2);
    drawChar(19 + (40*userIndex),HEIGHT/2+16,'^',WHITE,BLACK,2);

    int gameDone = 0;

    while (!gameDone){

        if (decodingIndex == EXPECTEDFALLINGEDGES){
            decodingIndex = 0;
            SysTickDisable();
            GPIOIntDisable(IR_GPIO_PORT, IR_GPIO_PIN);
            int ButtonResult = evaluateCapturedPulses();
            if (ButtonResult == -1){
                ;
            }
            else if (ButtonResult == 11){ //Mute, move left
                //Erases current pointer
                drawChar(19 + (40*userIndex),HEIGHT/2+16,'^',BLACK,BLACK,2);

                if (userIndex == 0){
                    userIndex = 2; //Wrap to 3rd digit
                } else{
                    userIndex -= 1;
                }
                drawChar(19 + (40*userIndex),HEIGHT/2+16,'^',WHITE,BLACK,2);
            } else if (ButtonResult == 10){ //Last, move right
                //Erases current pointer
                drawChar(19 + (40*userIndex),HEIGHT/2+16,'^',BLACK,BLACK,2);
                if (userIndex == 2){
                    userIndex = 0; //Wrap to 0th digit
                } else{
                    userIndex += 1;
                }
                drawChar(19 + (40*userIndex),HEIGHT/2+16,'^',WHITE,BLACK,2);
            } else if (ButtonResult == 0){ //Check answers

                int i = 0;
                int correctDigits = 0;
                int lowerDigits = 0;
                int higherDigits = 0;
                for (i = 0; i < 3; i++){
                    if (trueAnswer[i] == userAnswer[i]){
                        correctDigits += 1;
                    } else if (trueAnswer[i] < userAnswer[i]){
                        higherDigits += 1;
                    } else if (trueAnswer[i] > userAnswer[i]){
                        lowerDigits += 1;
                    }
                }
                //Erase the current strings
                fillRect(0,HEIGHT/4 - 9, 18 * 6, 17, BLACK);

                setCursor(0,HEIGHT/4 - 9);
                char correctHigherStr[50];
                sprintf(correctHigherStr, "%i Correct, %i High,", correctDigits, higherDigits);
                Outstr(correctHigherStr);

                setCursor(0,HEIGHT/4);
                char lowerStr[50];
                sprintf(lowerStr, "%i Low", lowerDigits);
                Outstr(lowerStr);

                if (correctDigits == 3){
                    gameDone = 1;
                    playerScore += 2000;
                    updateScore();
                    fillScreen(BLACK);
                } else{
                    playerScore -= 200;
                    updateScore();
                }

            } else{ //Must be nums 1-9
                //Erase the pointed number
                drawChar(19 + (40*userIndex),HEIGHT/2-8,userAnswer[userIndex] + '0',BLACK,BLACK,2);
                drawChar(19 + (40*userIndex),HEIGHT/2-8,ButtonResult + '0',WHITE,BLACK,2);
                userAnswer[userIndex] = ButtonResult;
            }
        }

    }


}

void memoryGame(){
    fillScreen(BLACK);
    setCursor(19, 0);
    Outstr("Memorize arrows");
    updateScore();

    //Up arrow, 0
    //drawChar(61,HEIGHT/4-8, 24,WHITE,BLACK,2);
    //Down arrow, 1
    //drawChar(61,3*HEIGHT/4-8, 25,WHITE,BLACK,2);
    //Left arrow, 2
    //drawChar(19,HEIGHT/2-8,27,WHITE,BLACK,2);
    //Right arrow, 3
    //drawChar(103,HEIGHT/2-8,26,WHITE,BLACK,2);

    int round0[3] = {generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3)};
    int round1[5] = {generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3)};
    int round2[7] = {generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3)};
    int correctAnswers = 0;

    //Round 1
    setCursor(0,3*HEIGHT/4);
    Outstr("In");
    setCursor(0,3*HEIGHT/4+8);
    Outstr("sequence...");
    int i = 0;

    for (i = 0; i<3; i++){
        if (round0[i] == 0){
            drawChar(61,HEIGHT/4-8, 24,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(61,HEIGHT/4-8, 24,BLACK,BLACK,2);
        } else if (round0[i] == 1){
            drawChar(61,3*HEIGHT/4-8, 25,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(61,3*HEIGHT/4-8, 25,BLACK,BLACK,2);
        } else if (round0[i] == 2){
            drawChar(19,HEIGHT/2-8,27,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(19,HEIGHT/2-8,27,BLACK,BLACK,2);
        } else if (round0[i] == 3){
            drawChar(103,HEIGHT/2-8,26,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(103,HEIGHT/2-8,26,BLACK,BLACK,2);
        }
        MAP_UtilsDelay(10000000);
    }
    setTextColorTransparent(BLACK);
    setCursor(0,3*HEIGHT/4);
    Outstr("In");
    setTextColorTransparent(BLACK);
    setCursor(0,3*HEIGHT/4+8);
    Outstr("sequence...");

    int acceptingUserInput = 1;
    int answerTracker = 0;

    while (acceptingUserInput){
        if (decodingIndex == EXPECTEDFALLINGEDGES){
            decodingIndex = 0;
            SysTickDisable();
            GPIOIntDisable(IR_GPIO_PORT, IR_GPIO_PIN);
            int ButtonResult = evaluateCapturedPulses();
            int validAnswer = 1;

            if (ButtonResult == 2){ //Up arrow
                drawChar(61,HEIGHT/4-8, 24,WHITE,BLACK,2);
                if (round0[answerTracker] == 0){
                    playerScore += 100;
                    correctAnswers +=1;
                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 4){ //Left arrow
                drawChar(19,HEIGHT/2-8,27,WHITE,BLACK,2);
                if (round0[answerTracker] == 2){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 6){ //Right arrow
                drawChar(103,HEIGHT/2-8,26,WHITE,BLACK,2);
                if (round0[answerTracker] == 3){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 8){ //Down arrow
                drawChar(61,3*HEIGHT/4-8, 25,WHITE,BLACK,2);
                if (round0[answerTracker] == 1){
                    playerScore += 100;
                }
                else{
                    playerScore -= 100;
                }
            } else{
                validAnswer = 0;
            }

            if (validAnswer){
                MAP_UtilsDelay(10000000);
                updateScore();

                //Erase whatever arrow was drawn
                drawChar(61,HEIGHT/4-8, 24,BLACK,BLACK,2);
                drawChar(61,3*HEIGHT/4-8, 25,BLACK,BLACK,2);
                drawChar(19,HEIGHT/2-8,27,BLACK,BLACK,2);
                drawChar(103,HEIGHT/2-8,26,BLACK,BLACK,2);
                answerTracker += 1;
                if (answerTracker == 3){
                    acceptingUserInput = 0;
                }
            }

        }


    }

    //Round 2
    setCursor(0,3*HEIGHT/4);
    Outstr("In");
    setCursor(0,3*HEIGHT/4+8);
    Outstr("sequence...");
    i = 0;

    for (i = 0; i<5; i++){
        if (round1[i] == 0){
            drawChar(61,HEIGHT/4-8, 24,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(61,HEIGHT/4-8, 24,BLACK,BLACK,2);
        } else if (round1[i] == 1){
            drawChar(61,3*HEIGHT/4-8, 25,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(61,3*HEIGHT/4-8, 25,BLACK,BLACK,2);
        } else if (round1[i] == 2){
            drawChar(19,HEIGHT/2-8,27,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(19,HEIGHT/2-8,27,BLACK,BLACK,2);
        } else if (round1[i] == 3){
            drawChar(103,HEIGHT/2-8,26,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(103,HEIGHT/2-8,26,BLACK,BLACK,2);
        }
        MAP_UtilsDelay(10000000);
    }
    setTextColorTransparent(BLACK);
    setCursor(0,3*HEIGHT/4);
    Outstr("In");
    setTextColorTransparent(BLACK);
    setCursor(0,3*HEIGHT/4+8);
    Outstr("sequence...");


    acceptingUserInput = 1;
    answerTracker = 0;

    while (acceptingUserInput){
        if (decodingIndex == EXPECTEDFALLINGEDGES){
            decodingIndex = 0;
            SysTickDisable();
            GPIOIntDisable(IR_GPIO_PORT, IR_GPIO_PIN);
            int ButtonResult = evaluateCapturedPulses();
            int validAnswer = 1;

            if (ButtonResult == 2){ //Up arrow
                drawChar(61,HEIGHT/4-8, 24,WHITE,BLACK,2);
                if (round1[answerTracker] == 0){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 4){ //Left arrow
                drawChar(19,HEIGHT/2-8,27,WHITE,BLACK,2);
                if (round1[answerTracker] == 2){
                    playerScore += 100;
                    correctAnswers +=1;
                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 6){ //Right arrow
                drawChar(103,HEIGHT/2-8,26,WHITE,BLACK,2);
                if (round1[answerTracker] == 3){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 8){ //Down arrow
                drawChar(61,3*HEIGHT/4-8, 25,WHITE,BLACK,2);
                if (round1[answerTracker] == 1){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else{
                validAnswer = 0;
            }

            if (validAnswer){
                MAP_UtilsDelay(10000000);
                updateScore();
                //Erase whatever arrow was drawn
                drawChar(61,HEIGHT/4-8, 24,BLACK,BLACK,2);
                drawChar(61,3*HEIGHT/4-8, 25,BLACK,BLACK,2);
                drawChar(19,HEIGHT/2-8,27,BLACK,BLACK,2);
                drawChar(103,HEIGHT/2-8,26,BLACK,BLACK,2);
                answerTracker += 1;
                if (answerTracker == 5){
                    acceptingUserInput = 0;
                }
            }


        }

    }

    //Round 3
    setCursor(0,3*HEIGHT/4);
    Outstr("In");
    setCursor(0,3*HEIGHT/4+8);
    Outstr("sequence...");
    i = 0;

    for (i = 0; i<7; i++){
        if (round2[i] == 0){
            drawChar(61,HEIGHT/4-8, 24,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(61,HEIGHT/4-8, 24,BLACK,BLACK,2);
        } else if (round2[i] == 1){
            drawChar(61,3*HEIGHT/4-8, 25,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(61,3*HEIGHT/4-8, 25,BLACK,BLACK,2);
        } else if (round2[i] == 2){
            drawChar(19,HEIGHT/2-8,27,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(19,HEIGHT/2-8,27,BLACK,BLACK,2);
        } else if (round2[i] == 3){
            drawChar(103,HEIGHT/2-8,26,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(103,HEIGHT/2-8,26,BLACK,BLACK,2);
        }
        MAP_UtilsDelay(10000000);
    }
    setTextColorTransparent(BLACK);
    setCursor(0,3*HEIGHT/4);
    Outstr("In");
    setTextColorTransparent(BLACK);
    setCursor(0,3*HEIGHT/4+8);
    Outstr("sequence...");


    acceptingUserInput = 1;
    answerTracker = 0;

    while (acceptingUserInput){
        if (decodingIndex == EXPECTEDFALLINGEDGES){
            decodingIndex = 0;
            SysTickDisable();
            GPIOIntDisable(IR_GPIO_PORT, IR_GPIO_PIN);
            int ButtonResult = evaluateCapturedPulses();
            int validAnswer = 1;


            if (ButtonResult == 2){ //Up arrow
                drawChar(61,HEIGHT/4-8, 24,WHITE,BLACK,2);
                if (round2[answerTracker] == 0){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 4){ //Left arrow
                drawChar(19,HEIGHT/2-8,27,WHITE,BLACK,2);
                if (round2[answerTracker] == 2){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 6){ //Right arrow
                drawChar(103,HEIGHT/2-8,26,WHITE,BLACK,2);
                if (round2[answerTracker] == 3){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 8){ //Down arrow
                drawChar(61,3*HEIGHT/4-8, 25,WHITE,BLACK,2);
                if (round2[answerTracker] == 1){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else{
                validAnswer = 0;
            }

            if (validAnswer){
                MAP_UtilsDelay(10000000);
                updateScore();
                //Erase whatever arrow was drawn
                drawChar(61,HEIGHT/4-8, 24,BLACK,BLACK,2);
                drawChar(61,3*HEIGHT/4-8, 25,BLACK,BLACK,2);
                drawChar(19,HEIGHT/2-8,27,BLACK,BLACK,2);
                drawChar(103,HEIGHT/2-8,26,BLACK,BLACK,2);
                answerTracker += 1;
                if (answerTracker == 7){
                    acceptingUserInput = 0;
                }
            }


        }

    }




    char correctStr[50];
    sprintf(correctStr, "%i/15", correctAnswers);
    setCursor(WIDTH/2-12,HEIGHT/2);
    Outstr(correctStr);

    MAP_UtilsDelay(20000000);
    fillScreen(BLACK);



}


void leaderBoardSubmission(){


    fillScreen(BLACK);
    setCursor(0,0);
    Outstr("Leaderboard Entry");
    setCursor(0, 16);
    Outstr("Name: ");
    setCursor(0, 32);
    char scoreStr[50];
    sprintf(scoreStr, "Score: %i", playerScore);
    Outstr(scoreStr);


    while (1) {

          if (decodingIndex == EXPECTEDFALLINGEDGES){
              decodingIndex = 0;
              SysTickDisable();
              GPIOIntDisable(IR_GPIO_PORT, IR_GPIO_PIN);
              leaderBoardTyping();
          }

          if (systickexpired == 1 && currentCharacter != '\0'){
              addToBuf();
              buttonpresses = 0;
          }

      }



}


void initMainMenu(){
    fillScreen(BLACK);
    Outstr("Play Game");
    drawChar(54, 0, '<', WHITE, BLACK, 1);
    setCursor(0,20);
    Outstr("View Leaderboard");
    decodingIndex = 0;
    int mainMenuIndex = 0;
    playerScore = 0;
    while (1) {

        if (decodingIndex == EXPECTEDFALLINGEDGES){
            decodingIndex = 0;
            SysTickDisable();
            GPIOIntDisable(IR_GPIO_PORT, IR_GPIO_PIN);
            int ButtonResult = evaluateCapturedPulses();
            if (ButtonResult == 2 || ButtonResult == 8){
                //Must change this if we have more options
                if (mainMenuIndex == 0){
                    mainMenuIndex = 1;
                    drawChar(54, 0, '<', BLACK, BLACK, 1);
                    drawChar(96, 20, '<', WHITE, BLACK, 1);

                } else{
                    mainMenuIndex = 0;
                    drawChar(96, 20, '<', BLACK, BLACK, 1);
                    drawChar(54, 0, '<', WHITE, BLACK, 1);
                }

            }

            if (ButtonResult == 11){
                if (mainMenuIndex == 0){
                    accelCarMinigame();
                    passwordMinigame();
                    memoryGame();
                    leaderBoardSubmission();


                }
                else if (mainMenuIndex == 1){
                    //View leaderboard logic
                    UART_PRINT("View leaderboard logic");
                }
            }



        }



    }
}


void main() {
    //
    // Initialize board configuration
    //
    BoardInit();

    PinMuxConfig();
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    SpiConfig();

    SysTickInit();

    InitTerm();
    ClearTerm();

    Adafruit_Init();
    HandlerConfig();
    seedRandomGenerator();
    initMainMenu();



}
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************




static int http_post(int iTLSSockID){
    char acSendBuff[512];
    char acRecvbuff[1460];
    char cCLLength[200];
    char* pcBufHeaders;
    int lRetVal = 0;

    pcBufHeaders = acSendBuff;
    strcpy(pcBufHeaders, POSTHEADER);
    pcBufHeaders += strlen(POSTHEADER);
    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);
    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);
    strcpy(pcBufHeaders, "\r\n\r\n");

    int dataLength = strlen(DATA1);

    strcpy(pcBufHeaders, CTHEADER);
    pcBufHeaders += strlen(CTHEADER);
    strcpy(pcBufHeaders, CLHEADER1);

    pcBufHeaders += strlen(CLHEADER1);
    sprintf(cCLLength, "%d", dataLength);

    strcpy(pcBufHeaders, cCLLength);
    pcBufHeaders += strlen(cCLLength);
    strcpy(pcBufHeaders, CLHEADER2);
    pcBufHeaders += strlen(CLHEADER2);

    strcpy(pcBufHeaders, DATA1);
    pcBufHeaders += strlen(DATA1);

    int testDataLength = strlen(pcBufHeaders);

    UART_PRINT(acSendBuff);


    //
    // Send the packet to the server */
    //
    lRetVal = sl_Send(iTLSSockID, acSendBuff, strlen(acSendBuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("POST failed. Error Number: %i\n\r",lRetVal);
        sl_Close(iTLSSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }
    lRetVal = sl_Recv(iTLSSockID, &acRecvbuff[0], sizeof(acRecvbuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("Received failed. Error Number: %i\n\r",lRetVal);
        //sl_Close(iSSLSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }
    else {
        acRecvbuff[lRetVal+1] = '\0';
        UART_PRINT(acRecvbuff);
        UART_PRINT("\n\r\n\r");
    }

    return 0;
}

static int http_get(int iTLSSockID){
    char acSendBuff[512];
    char acRecvbuff[1460];
    char* pcBufHeaders;
    int lRetVal = 0;

    pcBufHeaders = acSendBuff;
    strcpy(pcBufHeaders, GETHEADER);
    pcBufHeaders += strlen(GETHEADER);
    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);
    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);
    strcpy(pcBufHeaders, "\r\n\r\n");

    // Send the packet to the server
    lRetVal = sl_Send(iTLSSockID, acSendBuff, strlen(acSendBuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("GET failed. Error Number: %i\n\r",lRetVal);
        sl_Close(iTLSSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }

    // Receive the response from the server
    lRetVal = sl_Recv(iTLSSockID, &acRecvbuff[0], sizeof(acRecvbuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("Received failed. Error Number: %i\n\r",lRetVal);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }
    else {
        acRecvbuff[lRetVal+1] = '\0';
        UART_PRINT(acRecvbuff);
        UART_PRINT("\n\r\n\r");
    }

    return 0;
}
