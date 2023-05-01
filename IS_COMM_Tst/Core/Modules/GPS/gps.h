
#include "serial_user.h"


typedef enum _gps_packet_buffer_status {
  WAITING_FOR_PACKET_START,
  IN_HEADER,
  IN_PACKET,
  IN_CHECKSUM,
  PACKET_RECEIVED
} gps_packet_buffer_status;

typedef union {
  uint8_t rawBytes[21];
  struct {
    char messageID[7];
    float utcTime;
    float latitude;
    char ns_indic;
    float longitude;
    char ew_indic;
    uint8_t pos_fix_indic;
    uint8_t satellites;
    float HDOP;
    float msl_altitude;
    char alt_units;
    float geoidal_sep;
    char geoid_units;
    uint8_t age_diff_corr;
    uint8_t checksum;
  } values;
} gpsDataStruct;

extern uint8_t gpsPacketBuffer[];
extern uint8_t gpsHeaderBuffer[];

extern uint8_t gpsPacketBufferIndex;
extern uint8_t gpsHeaderBufferIndex;

extern gps_packet_buffer_status gpsPacketStatus;

extern uint8_t gpsProcessPacket;

extern gpsDataStruct gpsData0;
extern uint8_t valid_GPS;

void ProcessGpsInputChar(comm_buffer_t * _buff_instance);
uint8_t ParsePacket (uint8_t *_packet2Check, uint8_t _packetLength, uint8_t _paramCount);
void ProcessGPS_Packet(void);

#define GPS_PACKET_BUFFER_LENGTH 150