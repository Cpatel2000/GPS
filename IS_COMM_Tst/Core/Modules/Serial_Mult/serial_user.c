// serial_user.c

#include "Serial.h"
#include "serial_user.h"
#include "ASCII_numbers.h"
#include <stdio.h>
#include "fiber_Interface.h"
#include "RS_485_Interface.h"
#include "gps.h"
#include "ux_manager.h"


uint8_t GPS_rxBuffer[RX_BUFFER_SIZE];
uint8_t GPS_txBuffer[TX_BUFFER_SIZE];

comm_buffer_t gps_inputBuffer = {0,0, GPS_rxBuffer, RX_BUFFER_SIZE, USART1};
comm_buffer_t gps_outputBuffer = {0,0, GPS_txBuffer, TX_BUFFER_SIZE, USART1};

// PC circular buffer
uint8_t PC_rxBuffer[PC_RX_BUFFER_SIZE];
uint8_t PC_txBuffer[PC_TX_BUFFER_SIZE];
comm_buffer_t pc_inputBuffer = {0,0, PC_rxBuffer, PC_RX_BUFFER_SIZE, USART2};
comm_buffer_t pc_outputBuffer = {0,0, PC_txBuffer, PC_TX_BUFFER_SIZE, USART2};

// PC packet buffer
uint8_t pcPacketBuffer[PC_PACKET_BUFFER_LENGTH] = "";
uint8_t pcPacketBufferIndex = 0;
pc_packet_buffer_status pcPacketStatus = PC_WAITING_FOR_PACKET_START;

uint8_t pcProcessPacket = false;

uint8_t packetBuffer[16];
uint8_t inPacket = false;
uint8_t nextPacketChar = 0;
uint8_t processPacket = false;

uint8_t processBinaryPacket = false;
int16_t packetLength = -1;
uint8_t escapeDetected = false;


extern uint8_t flashLED;
extern uint8_t buttonPushed;
extern uint16_t flashDelay;
extern uint16_t flashDelaySeed;
extern uint8_t flashAtSpeed;
extern uint16_t serialValue;
extern uint16_t targetRemote;

//----- CODE START --------------------
void ProcessPcInputChar(comm_buffer_t * _buff_instance)
{
  switch (pcPacketStatus) {
  case PC_WAITING_FOR_PACKET_START:
    if (_buff_instance->buffer[_buff_instance->nextBufferOut] == '$') {
      pcPacketBufferIndex = 1;
      pcPacketBuffer[0] = _buff_instance->buffer[_buff_instance->nextBufferOut];
      
      pcPacketStatus = PC_IN_PACKET;
    }
    break;
  case PC_IN_PACKET:
    pcPacketBuffer[pcPacketBufferIndex] = _buff_instance->buffer[_buff_instance->nextBufferOut];
    
    if ((pcPacketBuffer[pcPacketBufferIndex] == '\r') || (pcPacketBuffer[pcPacketBufferIndex] == '\n')) {
      pcProcessPacket = true;
      
      pcPacketStatus = PC_WAITING_FOR_PACKET_START;
    }
    
    if (pcPacketBufferIndex >= PC_PACKET_BUFFER_LENGTH - 1)
      pcPacketStatus = PC_WAITING_FOR_PACKET_START;
    
    
    pcPacketBufferIndex++;
    break;
//  case PC_IN_CHECKSUM:
//    // do the checksum
//    
//    //then set the flag to process if all is ok
//    pcProcessPacket = true;
//    pcPacketStatus = PC_WAITING_FOR_PACKET_START;
//    break;
  }
  
  _buff_instance->nextBufferOut++;
  if (_buff_instance->nextBufferOut >= _buff_instance->bufferLength) _buff_instance->nextBufferOut = 0;
}

void ProcessPcPacket(void)
{
  char message[25] = "";
  ConvertASCII2UINT16((char const *)&pcPacketBuffer[1], 1, '\n', &targetRemote);
  switch (pcPacketBuffer[2]) {
    case 'f':     // f = Send value to Fiber then to RS485 to display value
    case 'F':
      ConvertASCII2UINT16((char const *)&pcPacketBuffer[3], 5, '\n', &serialValue);
      sprintf(message, "$%df%d\n", targetRemote, serialValue);
      // Send string to Fiber optic (USART4)
      SendString(&fc_outputBuffer, message, sizeof(message), StripZeros, NoAddCRLF); // Send to fc_outputbuffer
      break;
    
    case 'l':     // l = turn on LED
    case 'L':
      sprintf(message, "$%dl\n", targetRemote);
      // Send string to RS485 (USART4)
      SendString(&rs485_outputBuffer, message, sizeof(message), StripZeros, NoAddCRLF); // Send to rs485_outputbuffer
      break;
      
    case 'g':
    case 'G':
      if (gpsData0.values.longitude && gpsData0.values.latitude) {
        sprintf(message,"%.02f %c,%.02f %c\n", gpsData0.values.latitude, gpsData0.values.ns_indic, gpsData0.values.longitude, gpsData0.values.ew_indic);
      }
      else {
        sprintf(message,"No lock");
      }
      SendString(&pc_outputBuffer, message, sizeof(message), StripZeros, NoAddCRLF); // Send to pc_outputbuffer
      break;
    case 'o':     // O = check for the button press on Remote 2 and send GPS data to view gps data on OLED
    case 'O':
      sprintf(message, "$%do\n", targetRemote);
      // Send string to RS485 (USART4)
      SendString(&rs485_outputBuffer, message, sizeof(message), StripZeros, NoAddCRLF); // Send to fc_inputbuffer
      break;
  }
  
  pcProcessPacket = false;
  
}

//// function to process the input buffer with an ASCII-based protocol
//uint8_t ProcessReceiveBuffer(void)
//{
//  if (rxBuffer[nextSerialRx2Proc] == '$') {
//    inPacket = true;
//    packetBuffer[0] = rxBuffer[nextSerialRx2Proc];
//    nextPacketChar = 1;
//  }
//  else {
//    if (inPacket == true) {
//      if (((rxBuffer[nextSerialRx2Proc] >= '0') && (rxBuffer[nextSerialRx2Proc] <= '9')) || 
//          ((rxBuffer[nextSerialRx2Proc] >= 'r') && (rxBuffer[nextSerialRx2Proc] <= 'v')) ||
//          ((rxBuffer[nextSerialRx2Proc] >= 'R') && (rxBuffer[nextSerialRx2Proc] <= 'V')) ||
//          (rxBuffer[nextSerialRx2Proc] >= '\n') || (rxBuffer[nextSerialRx2Proc] <= '\r')) {
//        
//            packetBuffer[nextPacketChar++] = rxBuffer[nextSerialRx2Proc];
//
//            if (rxBuffer[nextSerialRx2Proc] == '\n') {
//              processPacket = true; 
//              inPacket = false;
//            }
//          }
//      else {
//        inPacket = false;
//      }
//    }
//  }
//  
//  
//  if (++nextSerialRx2Proc >= RX_BUFFER_SIZE) {
//    nextSerialRx2Proc = 0;
//  }
//  return 0;
//
//}
//
//
//uint8_t ProcessPacket(void)
//{
//  switch (packetBuffer[1]) {
//  // list of commands
//  // each command has intentional fallthru for upper/lower case
//  case 'r':     // r = turn on LED
//  case 'R':     
////    HAL_GPIO_WritePin(BOARD_MOUNTED_LED, GPIO_PIN_SET);
//    flashLED = false;
//    flashAtSpeed = false;
//    break;
//  case 's':     // s = turn off LED
//  case 'S':
////    HAL_GPIO_WritePin(BOARD_MOUNTED_LED, GPIO_PIN_RESET);
//    flashLED = false;
//    flashAtSpeed = false;
//  break;
//  case 't':     // t = toggle LED
//  case 'T':
////    HAL_GPIO_TogglePin(BOARD_MOUNTED_LED);
//    flashLED = false;
//    flashAtSpeed = false;
//    break;
//  case 'u':     // u = flash LED at speed (the number entered is the on and off time in 10mS 
//  case 'U':     // increments, so a value of 100 is 1 sec on and 1 sec off
//    flashLED = false;
//    flashAtSpeed = true;
//    ConvertASCII2UINT16((char const *)&packetBuffer[2], 5, '\n', &flashDelaySeed);
//    flashDelay = flashDelaySeed;
//    break;
//  case 'v':     // v = check switch press
//  case 'V':
//    if (buttonPushed == true) {
//      SendString("Pushed", 6, StripZeros, AddCRLF);
//      buttonPushed = false;      
//    }
//    else {
//      SendString("Not Pushed", 10, StripZeros, AddCRLF);
//    }
//    break;
//  case 'w':     // w = write a value to the variable "serialValue"
//  case 'W':
//    ConvertASCII2UINT16(&packetBuffer[2], 5, '\n', &serialValue);
//    break;
//  case 'x':     // x = flash LED at a 1/2Hz rate (1 sec on, 1 sec off)
//  case 'X':
//    flashLED = true;
//    flashAtSpeed = false;
//    break;
//  }
//
//  processPacket = false;
//
//  return 0;
//}
//
