// serial_user.c

#include <stdio.h>
#include "serial.h"
#include "serial_user.h"
#include "ASCII_numbers.h"
#include "SevenSegLED.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_lpuart.h"


// Modular Variables
uint8_t packetBuffer[16];
uint8_t inPacket = false;
uint8_t nextPacketChar = 0;
uint8_t processPacket = false;
uint8_t deviceID = 0;

UART_HandleTypeDef hlpuart1;


// external variables
extern uint8_t rxBuffer[];
extern uint8_t txBuffer[];
extern uint8_t remoteAddress;
extern uint8_t protocolSelection;
extern uint16_t displayNumber;

// ***********
// include externs here for variables that
//refer to things being controlled or changed by
//the packet processor.
extern uint8_t flashLED;
extern uint8_t buttonPushed;
extern uint16_t flashDelay;
extern uint16_t flashDelaySeed;
extern uint8_t flashAtSpeed;
extern uint16_t serialValue;


// ****** CODE START **********
// function to process the input buffer
uint8_t ProcessReceiveBuffer(void)
{
  if (rxBuffer[nextSerialRx2Proc] == '$') {
    inPacket = true;
    packetBuffer[0] = rxBuffer[nextSerialRx2Proc];
    nextPacketChar = 1;
  }
  else {
    if (inPacket == true) {
      packetBuffer[nextPacketChar++] = rxBuffer[nextSerialRx2Proc];

      if ( (rxBuffer[nextSerialRx2Proc] == '\n') && (packetBuffer[1] == 0x30 + remoteAddress) ) {
        processPacket = true;
        inPacket = false;
      }
    }
  }
  
  if (++nextSerialRx2Proc >= RX_BUFFER_SIZE) {
    nextSerialRx2Proc = 0;
  }
  return 0;

}


uint8_t ProcessPacket(void)
{
  char message[MSG_SIZE] = "";
  switch (packetBuffer[2]) {
  case 'd':     // n = Display a value
  case 'D':
    ConvertASCII2UINT16((const char *)&packetBuffer[3], 5, '\n', &displayNumber);
    ParseToDigits(displayNumber, NoDp, NotBlank);
    break;
  case 'l':     // l = turn on LED
  case 'L':
    HAL_GPIO_TogglePin(LED0_OUT); //toggle for now
    flashLED = false;
    break;
  case 'o':     // o = button Press Remote 2 -> change OLED to GPS
  case 'O':
    sprintf(message, "$%co%c\n",remoteAddress+0x30, buttonPushed ? 'p' : 'n');
    //sprintf(message, "$HELLO");
    SendString(message, sizeof(message), StripZeros, NoAddCRLF);
    //HAL_UART_Transmit(&hlpuart1, message, sizeof(message), 1000);
    buttonPushed = false;
    break;
  case 'r': // button Press Remote 1 -> LED remote 2
  case 'R':
    sprintf(message, "$%cr%c\n",remoteAddress+0x30, buttonPushed ? 'p' : 'n');
    SendString(message, sizeof(message), StripZeros, NoAddCRLF);
    break;
  }
 
  processPacket = false;

  return 0;
}


