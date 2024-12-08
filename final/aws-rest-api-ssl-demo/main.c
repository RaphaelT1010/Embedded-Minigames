
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
volatile int userTyping;
volatile int playerScore;


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



#define DATE                1    /* Current Date */
#define MONTH               6     /* Month 1-12 */
#define YEAR                2024  /* Current year */
#define HOUR                12   /* Time - hours */
#define MINUTE              7   /* Time - minutes */
#define SECOND              0     /* Time - seconds */


#define APPLICATION_NAME      "SSL"
#define APPLICATION_VERSION   "SQ24"
#define SERVER_NAME           "" // CHANGE ME
#define GOOGLE_DST_PORT       8443

#define GETHEADER "GET "
#define POSTHEADER "POST "             // CHANGE ME
#define HOSTHEADER "Host: "  // CHANGE ME
#define CHEADER "Connection: Keep-Alive\r\n"
#define CTHEADER "Content-Type: application/json; charset=utf-8\r\n"
#define CLHEADER1 "Content-Length: "
#define CLHEADER2 "\r\n\r\n"



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




    size_t templateLen = strlen(templateJSON);
    size_t inputLen = strlen(inputText);
    size_t newJSONLen = templateLen + inputLen + 1;

    char* newJSON = (char*)malloc(newJSONLen);
    if (newJSON == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    snprintf(newJSON, newJSONLen, templateJSON, inputText);

    return newJSON;
}


static int postCustomEmail(int iTLSSockID){
    char acSendBuff[512];
    char acRecvbuff[1460];
    char cCLLength[200];
    char* pcBufHeaders;
    int lRetVal = 0;

    char textForJson[50];
    sprintf(textForJson, "Player: %s Score: %i", bufferToSend, playerScore);
    const char* JSONtoSend = addTextToJSON(textForJson);


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

    long lRetVal = -1;

    g_app_config.host = SERVER_NAME;
    g_app_config.port = GOOGLE_DST_PORT;

    lRetVal = connectToAccessPoint();
    lRetVal = set_time();
    if(lRetVal < 0) {
        UART_PRINT("Unable to set time in the device");
        LOOP_FOREVER();
    }
    lRetVal = tls_connect();
    if(lRetVal < 0) {
        ERR_PRINT(lRetVal);
    }
    postCustomEmail(lRetVal);
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
    }
    else if (arraysEqual(currentButton, two, EXPECTEDFALLINGEDGES)){
        if (buttonpresses % 3 == 0){
            displayToSendMessageChar('a');
            currentCharacter = 'a';
        }
        else if (buttonpresses % 3 == 1){
            displayToSendMessageChar('b');
            currentCharacter = 'b';
        } else if (buttonpresses % 3 == 2){
            displayToSendMessageChar('c');
            currentCharacter = 'c';
        }
        buttonpresses = buttonpresses + 1;

    } else if (arraysEqual(currentButton,three, EXPECTEDFALLINGEDGES)){
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
        if (bufferIndex != 0){
            displayToSendMessageIndex--;
            displayToSendMessageChar(' ');
            bufferToSend[bufferIndex - 1] = '\0';
            bufferIndex--;
        }

    } else if (arraysEqual(currentButton,mute, EXPECTEDFALLINGEDGES)){
        sendEmail();
        bufferIndex = 0;
        fillScreen(BLACK);
        setCursor(0,HEIGHT/2);
        Outstr("Thanks for playing!");
        MAP_UtilsDelay(30000000);
        userTyping = 0;


    }else if (arraysEqual(currentButton,zero, EXPECTEDFALLINGEDGES)){
        currentCharacter = ' ';
    }
    else{
        currentCharacter = '\0';
        buttonpresses = 0;
    }

    //Flush current button too to be safe
    int i = 0;
    for (i = 0; i < EXPECTEDFALLINGEDGES; i++) {
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

    int i = 0;
    for (i = 0; i < EXPECTEDFALLINGEDGES; i++) {
        currentButton[i] = -1;
    }

    return -1;
}

static void decodeSignal(uint64_t tmp_delta_us, int count) {
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

        if (tmp_delta_us > 600){
            decodeSignal(tmp_delta_us, decodingIndex);
            decodingIndex++;
            SysTickReset();
        }
    }
    return;

}



void SpiConfig(){
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);
    InitTerm();
    ClearTerm();

    MAP_PRCMPeripheralReset(PRCM_GSPI);

    MAP_SPIReset(GSPI_BASE);


    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                           SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                           (SPI_SW_CTRL_CS |
                                   SPI_4PIN_MODE |
                                   SPI_TURBO_OFF |
                                   SPI_CS_ACTIVEHIGH |
                                   SPI_WL_8));


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

    //int round1[5] = {generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3)};
    int roundAnswers[10] = {generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3),generateRandomNumber(0,3), generateRandomNumber(0,3), generateRandomNumber(0,3), generateRandomNumber(0,3)};
    int correctAnswers = 0;


    setCursor(0,3*HEIGHT/4);
    Outstr("In");
    setCursor(0,3*HEIGHT/4+8);
    Outstr("sequence...");
    int i = 0;

    for (i = 0; i<10; i++){
        if (roundAnswers[i] == 0){
            drawChar(61,HEIGHT/4-8, 24,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(61,HEIGHT/4-8, 24,BLACK,BLACK,2);
        } else if (roundAnswers[i] == 1){
            drawChar(61,3*HEIGHT/4-8, 25,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(61,3*HEIGHT/4-8, 25,BLACK,BLACK,2);
        } else if (roundAnswers[i] == 2){
            drawChar(19,HEIGHT/2-8,27,WHITE,BLACK,2);
            MAP_UtilsDelay(10000000);
            drawChar(19,HEIGHT/2-8,27,BLACK,BLACK,2);
        } else if (roundAnswers[i] == 3){
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
                if (roundAnswers[answerTracker] == 0){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 4){ //Left arrow
                drawChar(19,HEIGHT/2-8,27,WHITE,BLACK,2);
                if (roundAnswers[answerTracker] == 2){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 6){ //Right arrow
                drawChar(103,HEIGHT/2-8,26,WHITE,BLACK,2);
                if (roundAnswers[answerTracker] == 3){
                    playerScore += 100;
                    correctAnswers +=1;

                }
                else{
                    playerScore -= 100;
                }
            } else if (ButtonResult == 8){ //Down arrow
                drawChar(61,3*HEIGHT/4-8, 25,WHITE,BLACK,2);
                if (roundAnswers[answerTracker] == 1){
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
                if (answerTracker == 10){
                    acceptingUserInput = 0;
                }
            }


        }

    }

    char correctStr[50];
    sprintf(correctStr, "%i/10", correctAnswers);
    setCursor(WIDTH/2-12,HEIGHT/2);
    Outstr(correctStr);

    MAP_UtilsDelay(20000000);
    fillScreen(BLACK);



}

void treasureHunt(){

    fillScreen(BLACK);
    setCursor(19, 0);
    Outstr("Treasure Hunt!");
    updateScore();

    int treasureX = generateRandomNumber(0,128);
    int treasureY = generateRandomNumber(8,120);
    int timesWon = 0;
    int numberofDigs = 0;

    int ballX = WIDTH/2;
    int ballY = HEIGHT/2;

    while (timesWon != 3){ //Must win 3 times
        drawBall(ballX,ballY);
        unsigned char oldX = ballX;
        unsigned char oldY = ballY;

        I2C_IF_Write(dvcAdr,&dvcRegOffSetX,1,0);
        I2C_IF_Read(dvcAdr, &rdDataBufX[0], 1);
        int dx = calculateDelta(rdDataBufX[0]);


        I2C_IF_Write(dvcAdr,&dvcRegOffSetY,1,0);
        I2C_IF_Read(dvcAdr, &rdDataBufY[0], 1);
        int dy = calculateDelta(rdDataBufY[0]);


        ballX = ballX + dy;
        ballY = ballY + dy;

        ballX = ballX + dx;
        if(ballX > WIDTH - BALLRADIUS)
            ballX = WIDTH - BALLRADIUS;
        if(ballX < BALLRADIUS)
            ballX= BALLRADIUS;
        if(ballY > HEIGHT - 12 - BALLRADIUS)
            ballY = HEIGHT - 12 - BALLRADIUS;
        if(ballY < BALLRADIUS + 12)
            ballY = BALLRADIUS + 12;

        //Erasing
        eraseBall(oldX, oldY);

        int eraseUp;
        int eraseDown;
        int eraseRight;
        int eraseLeft;

        if (decodingIndex == EXPECTEDFALLINGEDGES){
            decodingIndex = 0;
            eraseUp = 0;
            eraseDown = 0;
            eraseRight = 0;
            eraseLeft = 0;
            SysTickDisable();
            GPIOIntDisable(IR_GPIO_PORT, IR_GPIO_PIN);
            int ButtonResult = evaluateCapturedPulses();

            if (ButtonResult == 11){
                numberofDigs += 1;
                if (ballX > treasureX - 13 && ballX < treasureX + 13 && ballY > treasureY - 13 && ballY < treasureY + 13){
                    playerScore += 500;
                    updateScore();
                    treasureX = generateRandomNumber(0,128);
                    treasureY = generateRandomNumber(8,120);
                    drawChar(104 + (8 * timesWon), 0, '|', WHITE, BLACK, 1);
                    timesWon += 1;

                } else{
                    if (ballX <= treasureX - 13){
                        drawChar(ballX + 16,ballY,26,WHITE,BLACK,2); //Right
                        eraseRight = 1;
                    }

                    if (ballX >= treasureX + 13){
                        drawChar(ballX - 16,ballY,27,WHITE,BLACK,2); //Left
                        eraseLeft = 1;
                    }

                    if (ballY <= treasureY - 13){
                        drawChar(ballX,ballY + 16,25,WHITE,BLACK,2); //Down
                        eraseDown = 1;

                    }

                    if (ballY >= treasureY + 13){
                        drawChar(ballX,ballY - 16,24,WHITE,BLACK,2); //Up
                        eraseUp = 1;

                    }

                    MAP_UtilsDelay(20000000);

                    if (eraseRight){
                        drawChar(ballX + 16,ballY,26,BLACK,BLACK,2);
                    }

                    if (eraseLeft){
                        drawChar(ballX - 16,ballY,27,BLACK,BLACK,2);

                    }

                    if (eraseDown){
                        drawChar(ballX,ballY + 16, 25,BLACK,BLACK,2);

                    }

                    if (eraseUp){
                        drawChar(ballX,ballY - 16, 24,BLACK,BLACK,2);
                    }
                    playerScore -= 100;
                    updateScore();
                }
            }
        }
    }

    fillScreen(BLACK);

    char correctStr[50];
    sprintf(correctStr, "Took %i digs to find", numberofDigs);
    setCursor(0,HEIGHT/2);
    Outstr(correctStr);
    setCursor(0, HEIGHT/2 + 8);
    Outstr("3 treasures");
    MAP_UtilsDelay(30000000);

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
    userTyping = 1;
    displayToSendMessageIndex = 6;

    while (userTyping) {

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

    fillScreen(BLACK);



}

//Ball should start at center
int ballDropX = WIDTH/2;
int ballDropY = HEIGHT/2;

int goalX = WIDTH/2;
int goalY = HEIGHT - 20;

int scoreCounter = 0;
int goalSwitch = 0;


void ballDrop() {
    fillScreen(BLACK);
    setCursor(19, 0);
    Outstr("Drop in the Goal!");
    updateScore();
    while(1) {
        if(scoreCounter == 15) {
            break;
        }

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

        drawBall(ballDropX, ballDropY);

        //Save to erase later
        unsigned char oldX = ballDropX;
        unsigned char oldY = ballDropY;

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
        ballDropX = ballDropX + dx;
        if(ballDropX > WIDTH - BALLRADIUS) {
            ballDropX = WIDTH - BALLRADIUS;
            scoreCounter++;
            ballDropX = WIDTH/2;
            ballDropY = HEIGHT/2;
            playerScore -= 100;
            updateScore();
        }
        if(ballDropX < BALLRADIUS) {
            ballDropX = BALLRADIUS;
            scoreCounter++;
            ballDropX = WIDTH/2;
            ballDropY = HEIGHT/2;
            playerScore -= 100;
            updateScore();
        }

        ballDropY = ballDropY + dy;
        if(ballDropY > HEIGHT - BALLRADIUS - 12) {
            ballDropY = HEIGHT - BALLRADIUS - 12;
            scoreCounter++;
            ballDropX = WIDTH/2;
            ballDropY = HEIGHT/2;
            playerScore -= 100;
            updateScore();
        }
        if(ballDropY < BALLRADIUS + 12) {
            ballDropY = BALLRADIUS + 12;
            scoreCounter++;
            ballDropX = WIDTH/2;
            ballDropY = HEIGHT/2;
            playerScore -= 100;
            updateScore();
        }

        //Erasing the ball at the old location
        eraseBall(oldX, oldY);

        if((ballDropX < (goalX + 8) && ballDropX > (goalX - 8)) && (ballDropY < (goalY + 8) && ballDropY > (goalY - 8))) {
            playerScore += 100;
            updateScore();

            ballDropX = WIDTH/2;
            ballDropY = HEIGHT/2;
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
                goalY = HEIGHT - 20;
                goalX = WIDTH/2;
            }
        }
    }

    fillScreen(BLACK);

}

void createControllerLogo(){
    fillRect(12,24, 6, 6, WHITE);
    fillRect(18,18, 6, 6, WHITE);
    fillRect(24,12, 6, 6, WHITE);
    fillRect(30,12, 6, 6, WHITE);
    fillRect(36,12, 6, 6, WHITE);
    fillRect(42,12, 6, 6, WHITE);
    fillRect(48,12, 6, 6, WHITE);
    fillRect(54,12, 6, 6, WHITE);
    fillRect(60,12, 6, 6, WHITE);
    fillRect(66,12, 6, 6, WHITE);
    fillRect(72,12, 6, 6, WHITE);
    fillRect(78,12, 6, 6, WHITE);
    fillRect(84,12, 6, 6, WHITE);
    fillRect(90,12, 6, 6, WHITE);
    fillRect(96,12, 6, 6, WHITE);
    fillRect(102,18, 6, 6, WHITE);
    fillRect(108,24, 6, 6, WHITE);
    fillRect(12,30, 6, 6, WHITE);
    fillRect(12,36, 6, 6, WHITE);
    fillRect(12,42, 6, 6, WHITE);
    fillRect(12,48, 6, 6, WHITE);
    fillRect(12,54, 6, 6, WHITE);
    fillRect(18,60, 6, 6, WHITE);
    fillRect(24,66, 6, 6, WHITE);
    fillRect(108,30, 6, 6, WHITE);
    fillRect(108,36, 6, 6, WHITE);
    fillRect(108,42, 6, 6, WHITE);
    fillRect(108,48, 6, 6, WHITE);
    fillRect(108,54, 6, 6, WHITE);
    fillRect(102,60, 6, 6, WHITE);
    fillRect(96,66, 6, 6, WHITE);
    fillRect(30,66, 6, 6, WHITE);
    fillRect(90,66, 6, 6, WHITE);
    fillRect(36,60, 6, 6, WHITE);
    fillRect(42,54, 6, 6, WHITE);
    fillRect(84,60, 6, 6, WHITE);
    fillRect(78,54, 6, 6, WHITE);
    fillRect(42,54, 6, 6, WHITE);
    fillRect(48,54, 6, 6, WHITE);
    fillRect(54,54, 6, 6, WHITE);
    fillRect(60,54, 6, 6, WHITE);
    fillRect(66,54, 6, 6, WHITE);
    fillRect(72,54, 6, 6, WHITE);
    fillRect(78,54, 6, 6, WHITE);
    fillRect(90,36, 6, 6, RED);
    fillRect(72,36, 6, 6, GREEN);
    fillRect(48,36, 6, 6, YELLOW);
    fillRect(30,36, 6, 6, BLUE);
}

void initMainMenu(){
    fillScreen(BLACK);
    createControllerLogo();
    setCursor(18, HEIGHT - 50);
    setTextSize(1);
    Outstr("Embedded Games");
    setTextSize(1);
    setCursor(0,HEIGHT - 35);
    Outstr("Play Games");
    drawChar(60, HEIGHT - 35, '<', WHITE, BLACK, 1);
    setCursor(0,HEIGHT - 27);
    Outstr("How to Play");
    setCursor(0,HEIGHT - 16);
    Outstr("2");
    drawChar(8, HEIGHT - 16, 24, WHITE, BLACK, 1);
    setCursor(0,HEIGHT - 8);
    Outstr("8");
    drawChar(8, HEIGHT - 8, 25, WHITE, BLACK, 1);
    setCursor(WIDTH - 72,HEIGHT - 12);
    Outstr("Mute = Enter");
    setCursor(0,0);
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
                if (mainMenuIndex == 0){
                    mainMenuIndex = 1;
                    drawChar(60, HEIGHT - 35, '<', BLACK, BLACK, 1);
                    drawChar(66, HEIGHT - 27, '<', WHITE, BLACK, 1);

                } else{
                    mainMenuIndex = 0;
                    drawChar(66, HEIGHT - 27, '<', BLACK, BLACK, 1);
                    drawChar(60, HEIGHT - 35, '<', WHITE, BLACK, 1);
                }

            }

            if (ButtonResult == 11){
                if (mainMenuIndex == 0){
                    ballDrop();
                    accelCarMinigame();
                    passwordMinigame();
                    memoryGame();
                    treasureHunt();
                    leaderBoardSubmission();
                }
                else if (mainMenuIndex == 1){
                    viewInstructions();
                }
            }
        }
    }
}


void viewInstructions(){

    fillScreen(BLACK);


    setCursor(0,0);
    setTextColorTransparent(YELLOW);
    Outstr("Drop in the goal!");
    setTextColorTransparent(WHITE);
    setCursor(0,8);
    Outstr("Tilt accelerometer");
    setCursor(0,16);
    Outstr("to goals");

    setCursor(0,24);
    setTextColorTransparent(YELLOW);
    Outstr("Dodge the cars!");
    setCursor(0,32);
    setTextColorTransparent(WHITE);
    Outstr("Tilt accelerometer");
    setCursor(0,40);
    Outstr("up/down to dodge cars");

    setCursor(0,48);
    setTextColorTransparent(YELLOW);
    Outstr("Guess the code");
    setCursor(0,56);
    setTextColorTransparent(WHITE);
    Outstr("Mute/Last Left/Right");
    setCursor(0,64);
    Outstr("0 enter 1-9 input");

    setCursor(0,72);
    setTextColorTransparent(YELLOW);
    Outstr("Memorize arrows");
    setCursor(0,80);
    setTextColorTransparent(WHITE);
    Outstr("Up=2 Down=8");
    setCursor(0,88);
    Outstr("Left=4 Right=6");

    setCursor(0,96);
    setTextColorTransparent(YELLOW);
    Outstr("Treasure Hunt!");
    setCursor(0,104);
    setTextColorTransparent(WHITE);
    Outstr("Tilt accelerometer");
    setCursor(0,112);
    Outstr("Mute to scan");


    drawChar(0,HEIGHT - 8,27,WHITE,BLACK,1);
    drawChar(8,HEIGHT - 8,'0',WHITE,BLACK,1);
    setCursor(16,HEIGHT - 8);
    Outstr("Main Menu");
    while (1){
        if (decodingIndex == EXPECTEDFALLINGEDGES){
            decodingIndex = 0;
            SysTickDisable();
            GPIOIntDisable(IR_GPIO_PORT, IR_GPIO_PIN);
            int ButtonResult = evaluateCapturedPulses();

            if (ButtonResult == 0){
                initMainMenu();
            }
        }


    }
}

void main() {

    BoardInit();

    PinMuxConfig();

    I2C_IF_Open(I2C_MASTER_MODE_FST);

    SpiConfig();

    SysTickInit();

    Adafruit_Init();

    HandlerConfig();

    seedRandomGenerator();

    initMainMenu();



}
