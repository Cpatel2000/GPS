

#include "Serial.h"
#include "fiber_Interface.h"
#include "ASCII_numbers.h"
#include <stdio.h>
#include "RS_485_Interface.h"
#include "ux_manager.h"

// RS485 Interface
// RS485 circular buffer
uint8_t RS485_rxBuffer[RS485_RX_BUFFER_SIZE];
uint8_t RS485_txBuffer[RS485_TX_BUFFER_SIZE];

comm_buffer_t rs485_inputBuffer = { 0,0, RS485_rxBuffer, RS485_RX_BUFFER_SIZE, USART3 };
comm_buffer_t rs485_outputBuffer = { 0,0, RS485_txBuffer, RS485_TX_BUFFER_SIZE, USART3 };

// RS485 packet buffer
uint8_t rs485PacketBuffer[RS485_PACKET_BUFFER_LENGTH] = "";
uint8_t rs485PacketBufferIndex = 0;
rs485_packet_buffer_status rs485PacketStatus = RS485_WAITING_FOR_PACKET_START;

uint8_t rs485ProcessPacket = false;

extern uint16_t rs485Value;
extern uint16_t receiveRemoteAddress;

//----- CODE START --------------------
void ProcessRs485InputChar(comm_buffer_t* _buff_instance)
{
    switch (rs485PacketStatus) {
    case RS485_WAITING_FOR_PACKET_START:
        if (_buff_instance->buffer[_buff_instance->nextBufferOut] == '$') {
            rs485PacketBufferIndex = 1;
            rs485PacketBuffer[0] = _buff_instance->buffer[_buff_instance->nextBufferOut];

            rs485PacketStatus = RS485_IN_PACKET;
        }
        break;
    case RS485_IN_PACKET:
        rs485PacketBuffer[rs485PacketBufferIndex] = _buff_instance->buffer[_buff_instance->nextBufferOut];

        if ((rs485PacketBuffer[rs485PacketBufferIndex] == '\r') || (rs485PacketBuffer[rs485PacketBufferIndex] == '\n')) {
            rs485ProcessPacket = true;

            rs485PacketStatus = RS485_WAITING_FOR_PACKET_START;
        }

        if (rs485PacketBufferIndex >= RS485_PACKET_BUFFER_LENGTH - 1)
            rs485PacketStatus = RS485_WAITING_FOR_PACKET_START;


        rs485PacketBufferIndex++;
        break;
    }

        _buff_instance->nextBufferOut++;
        if (_buff_instance->nextBufferOut >= _buff_instance->bufferLength) _buff_instance->nextBufferOut = 0;
    }

    // just a stub for now...
    void ProcessRs485Packet(void)
    {
     
        char message[10] = "";
        ConvertASCII2UINT16((char const*)&rs485PacketBuffer[1], 1, '\n', &receiveRemoteAddress);
        switch (rs485PacketBuffer[2]) 
        case 'o':
        case 'O':
          if (rs485PacketBuffer[3] == 'p' && rs485PacketBuffer[1] == '1') {
            SwitchScreens(GPS_OLED);
          }
            break;
          
        case 'r':
        case 'R':

          if (rs485PacketBuffer[3] == 'p' && rs485PacketBuffer[1] == '0') {
            sprintf(message, "$1l\n");
            SendString(&rs485_outputBuffer, message, sizeof(message), StripZeros, NoAddCRLF);
          }
            break;
        }

        rs485ProcessPacket = false;

    }