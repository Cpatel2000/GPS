

#ifndef _FIBER_INTERFACE_
#define _FIBER_INTERFACE_


#include "serial.h"

#define FC_PACKET_BUFFER_LENGTH 50

#define FC_TX_BUFFER_SIZE 50
#define FC_RX_BUFFER_SIZE 30


// public references ********************
extern uint8_t fcProcessPacket;
extern comm_buffer_t fc_inputBuffer;
extern comm_buffer_t fc_outputBuffer;

// typedef,structure, union, enum definitions
typedef enum _fc_packet_buffer_status {
  FC_WAITING_FOR_PACKET_START,
  FC_IN_PACKET,
  FC_IN_CHECKSUM,
  FC_PACKET_RECEIVED
} fc_packet_buffer_status;

// prototypes

void ProcessFcInputChar(comm_buffer_t * _buff_instance);
void ProcessFcPacket(void);
//uint8_t fc_ProcessReceiveBuffer(void);
//uint8_t fc_ProcessReceiveByte(void);
//uint8_t fc_ProcessReceiveByteWithLength(void);

//uint8_t fc_ProcessPacket(void);
//void fc_ProcessBinaryPacket(void);

#endif