

#include "Serial.h"
#include "fiber_Interface.h"
#include "ASCII_numbers.h"
#include <stdio.h>
#include "serial_user.h"
#include "RS_485_Interface.h"

// FC Interface
// FC circular buffer
uint8_t FC_rxBuffer[FC_RX_BUFFER_SIZE];
uint8_t FC_txBuffer[FC_TX_BUFFER_SIZE];


comm_buffer_t fc_inputBuffer = {0,0, FC_rxBuffer, FC_RX_BUFFER_SIZE, USART4};
comm_buffer_t fc_outputBuffer = {0,0, FC_txBuffer, FC_TX_BUFFER_SIZE, USART4};


// FC packet buffer
uint8_t fcPacketBuffer[FC_PACKET_BUFFER_LENGTH] = "";
uint8_t fcPacketBufferIndex = 0;
fc_packet_buffer_status fcPacketStatus = FC_WAITING_FOR_PACKET_START;

uint8_t fcProcessPacket = false;
uint8_t sendFlag = false;

extern uint16_t fiberValue;
extern uint16_t ftargetRemote;

//----- CODE START --------------------
void ProcessFcInputChar(comm_buffer_t * _buff_instance)
{
  switch (fcPacketStatus) {
  case FC_WAITING_FOR_PACKET_START:
    if (_buff_instance->buffer[_buff_instance->nextBufferOut] == '$') {
      fcPacketBufferIndex = 1;
      fcPacketBuffer[0] = _buff_instance->buffer[_buff_instance->nextBufferOut];
      
      fcPacketStatus = FC_IN_PACKET;
    }
    break;
  case FC_IN_PACKET:
    fcPacketBuffer[fcPacketBufferIndex] = _buff_instance->buffer[_buff_instance->nextBufferOut];
    
    if ((fcPacketBuffer[fcPacketBufferIndex] == '\r') || (fcPacketBuffer[fcPacketBufferIndex] == '\n')) {
      fcProcessPacket = true;
      
      fcPacketStatus = FC_WAITING_FOR_PACKET_START;
    }
    
    if (fcPacketBufferIndex >= FC_PACKET_BUFFER_LENGTH - 1)
      fcPacketStatus = FC_WAITING_FOR_PACKET_START;
    
    
    fcPacketBufferIndex++;
    break;
  }
  
  _buff_instance->nextBufferOut++;
  if (_buff_instance->nextBufferOut >= _buff_instance->bufferLength) _buff_instance->nextBufferOut = 0;
}

// just a stub for now...
void ProcessFcPacket(void)
{

  
  char message[10] = "";
  ConvertASCII2UINT16((char const *)&fcPacketBuffer[1], 1, '\n', &ftargetRemote);
  switch (fcPacketBuffer[2]) {
  case 'f':
  case 'F':
    ConvertASCII2UINT16((char const *)&fcPacketBuffer[3], 5, '\n', &fiberValue);
    sprintf(message, "$%dd%d\n", ftargetRemote, fiberValue);
    SendString(&rs485_outputBuffer, message, sizeof(message), StripZeros, AddCRLF);
    sendFlag = true;
    break;
  }
  
  fcProcessPacket = false;
  
}