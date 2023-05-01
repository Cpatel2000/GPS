#ifndef _RS485_INTERFACE_
#define _RS485_INTERFACE_

#include "serial.h"

#define RS485_PACKET_BUFFER_LENGTH 50

#define RS485_TX_BUFFER_SIZE 50
#define RS485_RX_BUFFER_SIZE 30

// use for <buffer>_p_t
//#define <name>_BUFFER_SIZE 100


// public references ********************
extern uint8_t rs485ProcessPacket;
extern comm_buffer_t rs485_inputBuffer;
extern comm_buffer_t rs485_outputBuffer;

// typedef,structure, union, enum definitions
typedef enum _rs485_packet_buffer_status {
  RS485_WAITING_FOR_PACKET_START,
  RS485_IN_PACKET,
  RS485_IN_CHECKSUM,
  RS485_PACKET_RECEIVED
} rs485_packet_buffer_status;

// prototypes

void ProcessRs485InputChar(comm_buffer_t * _buff_instance);
void ProcessRs485Packet(void);

#endif