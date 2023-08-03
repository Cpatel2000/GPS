#include "gps.h"
#include "main.h"
#include "ux_manager.h"
#include "ASCII_numbers.h"
#include "string.h"


uint8_t gpsPacketBuffer[GPS_PACKET_BUFFER_LENGTH] = "$GNGGA";
uint8_t gpsHeaderBuffer[10];
char tmp_buff[10];

uint8_t gpsPacketBufferIndex = 0;
uint8_t gpsHeaderBufferIndex = 0;

gps_packet_buffer_status gpsPacketStatus = WAITING_FOR_PACKET_START;

uint8_t gpsProcessPacket = false;

gpsDataStruct gpsData0;

uint8_t checkSum_val;

void ProcessGpsInputChar(comm_buffer_t * _buff_instance)
{
  switch (gpsPacketStatus) {
  case WAITING_FOR_PACKET_START:
    if (_buff_instance->buffer[_buff_instance->nextBufferOut] == '$') {
      gpsHeaderBufferIndex = 0;
      gpsHeaderBuffer[gpsHeaderBufferIndex] = _buff_instance->buffer[_buff_instance->nextBufferOut];
      gpsHeaderBufferIndex++;
      gpsPacketStatus = IN_HEADER;
    }
    break;
  case IN_HEADER:
    gpsHeaderBuffer[gpsHeaderBufferIndex] = _buff_instance->buffer[_buff_instance->nextBufferOut];
    gpsHeaderBufferIndex++;
    
    if (gpsHeaderBufferIndex == 6) {
      if ((gpsHeaderBuffer[3] == 'G') && (gpsHeaderBuffer[4] == 'G') && (gpsHeaderBuffer[5] == 'A')) {
        gpsPacketBufferIndex = 6;
        gpsPacketStatus = IN_PACKET;
      }
      else 
        gpsPacketStatus = WAITING_FOR_PACKET_START;
    }
    break;
  case IN_PACKET:
    gpsPacketBuffer[gpsPacketBufferIndex] = _buff_instance->buffer[_buff_instance->nextBufferOut];
    
    if ((gpsPacketBuffer[gpsPacketBufferIndex] == '\r') || (gpsPacketBuffer[gpsPacketBufferIndex] == '\n'))
      gpsPacketStatus = WAITING_FOR_PACKET_START;
    
    if (gpsPacketBufferIndex >= GPS_PACKET_BUFFER_LENGTH - 1)
      gpsPacketStatus = WAITING_FOR_PACKET_START;
    
    if (gpsPacketBuffer[gpsPacketBufferIndex] == '*')
      gpsPacketStatus = IN_CHECKSUM;
    
    gpsPacketBufferIndex++;
    break;
  case IN_CHECKSUM:
    gpsPacketBuffer[gpsPacketBufferIndex] = _buff_instance->buffer[_buff_instance->nextBufferOut];
    
    if ((gpsPacketBuffer[gpsPacketBufferIndex] == '\r') || (gpsPacketBuffer[gpsPacketBufferIndex] == '\n')) {
      gpsProcessPacket = true;
      gpsPacketStatus = WAITING_FOR_PACKET_START;
    }
  
    if (gpsPacketBufferIndex >= GPS_PACKET_BUFFER_LENGTH - 1)
      gpsPacketStatus = WAITING_FOR_PACKET_START;

    gpsPacketBufferIndex++;
    break;
  }
  
  _buff_instance->nextBufferOut++;
  if (_buff_instance->nextBufferOut >= _buff_instance->bufferLength) _buff_instance->nextBufferOut = 0;
}

uint8_t ParsePacket (uint8_t *_packet2Check, uint8_t _packetLength, uint8_t _paramCount)
{
  uint8_t currentSeparator = 0;
  uint8_t nextSeparator = 0;
  uint8_t count = 1;
  //uint8_t currChar;
  uint8_t paramNumber = 0;
  //uint8_t i = 0;
  uint16_t temp_buff16;
  int32_t temp_buff32;
  float float_buff;
  

  while ( (count < _packetLength) && (_packet2Check[nextSeparator] != '\n') && (_packet2Check[nextSeparator] != 0) ) {
    nextSeparator++;
    for (int i = 0; i < 10; i++) {
      tmp_buff[i] = 0;
    }
    if ( (_packet2Check[nextSeparator] == ',') || (_packet2Check[nextSeparator] == '*') || (_packet2Check[nextSeparator] == '\n') || (_packet2Check[nextSeparator] == '\r')) {
      
      strncpy(tmp_buff, (const char *)&_packet2Check[currentSeparator], nextSeparator-currentSeparator);
     
      switch (paramNumber) {
      case 0:
        strncpy(gpsData0.values.messageID, (const char *) tmp_buff, nextSeparator-currentSeparator);
        
        break;
      case 1:
        GPSConvertASCII2FLOAT((char const *)&_packet2Check[currentSeparator], nextSeparator-currentSeparator, &float_buff);
        gpsData0.values.utcTime = float_buff;
        break;
      case 2:
        GPSConvertASCII2FLOAT((char const *)&_packet2Check[currentSeparator], nextSeparator-currentSeparator, &float_buff);
        gpsData0.values.latitude = float_buff;
        break;
      case 3:
        gpsData0.values.ns_indic = tmp_buff[0];
        break;
      case 4:
        GPSConvertASCII2FLOAT((char const *)&_packet2Check[currentSeparator], nextSeparator-currentSeparator, &float_buff);
        gpsData0.values.longitude = float_buff;
        break;
      case 5:
        gpsData0.values.ew_indic = tmp_buff[0];
        break;
      case 6:
        GPSConvertASCII2UINT16((char const *)&_packet2Check[currentSeparator], nextSeparator-currentSeparator, &temp_buff16);
        gpsData0.values.pos_fix_indic = (uint8_t) temp_buff16;
        break;
      case 7:
        GPSConvertASCII2UINT16((char const *)&_packet2Check[currentSeparator], nextSeparator-currentSeparator, &temp_buff16);
        gpsData0.values.satellites = (uint8_t) temp_buff16;
        break;
      case 8:
        GPSConvertASCII2FLOAT((char const *)&_packet2Check[currentSeparator], nextSeparator-currentSeparator, &float_buff);
        gpsData0.values.HDOP = float_buff;
        break;
      case 9:
        GPSConvertASCII2FLOAT((char const *)&_packet2Check[currentSeparator], nextSeparator-currentSeparator, &float_buff);
        gpsData0.values.msl_altitude = float_buff;
        break;
      case 10:
        gpsData0.values.alt_units = tmp_buff[0];
        break;
      case 11:
        GPSConvertASCII2FLOAT((char const *)&_packet2Check[currentSeparator], nextSeparator-currentSeparator, &float_buff);
        gpsData0.values.geoidal_sep = float_buff;
        break;
      case 12:
        gpsData0.values.geoid_units = tmp_buff[0];
        break;
      case 13: // age of diff corr.
        break; 
      case 14:
        GPSConvertHEX2DEC((char const *)&_packet2Check[currentSeparator+1], 2, &temp_buff16);
        gpsData0.values.checksum = (uint8_t) temp_buff16;
        break;
      }
    
     
      paramNumber++;
      if (_packet2Check[nextSeparator+1] != ',') {
        currentSeparator = nextSeparator + 1;
      } else {
        currentSeparator = nextSeparator;
      }
    }
  }
  return 0;
}

uint8_t GPSCheckCheckSum(uint8_t * _packet2Check, uint8_t _calc) 
{
  uint16_t chk = 0;
  uint8_t chksum;
  uint8_t count = 1;
  uint8_t currChar;
  uint8_t err = 0;
  
  currChar = _packet2Check[0];
  chksum = _packet2Check[0];
  
  if (_packet2Check[0] == '$') {
    currChar = _packet2Check[1];
    chksum = _packet2Check[1];
    count = 2;
  }
  
  while ( (currChar != '\n') && (currChar != 0) && (currChar != '*') ) {
    chksum ^= _packet2Check[count];
    count++;
    currChar = _packet2Check[count];
  }
  
      
      if (_packet2Check[count] == '*') {
        err = 0;
        GPSConvertHEX2DEC(&_packet2Check[count+1], 2, (uint16_t *) &chk);
      }
      else {
        err = 255;
      }
  
  
  if (_packet2Check[count] == 0) {
    err = 1;
  }
  
  if (err == 0) {
    if (_calc == true) {
      return chksum;
    }
    else {
      return (chksum == chk);
    }
  }
  else {
    return 255;
  }
}

void ProcessGPS_Packet(void)
{
  gpsIcon.data = '*';
  // Do your stuff :-)
  ParsePacket(gpsPacketBuffer,74,15);
  char tmp[1];
  strcpy(tmp, (char const *)gpsData0.values.ew_indic);
  if (strcmp((char const *)gpsData0.values.ns_indic, (char const *)"S")==0) {
    gpsLon.data = 0.0-gpsData0.values.longitude;
  }
  else {
    gpsLon.data = gpsData0.values.longitude;
  }
  if (strcmp(tmp,"W")==0) {
    gpsLat.data = 0.0-gpsData0.values.latitude;
  }
  else {
    gpsLat.data = gpsData0.values.latitude;
  }
  
  checkSum_val = GPSCheckCheckSum(gpsPacketBuffer, 0);
  if (checkSum_val == 1){
    valid_GPS = true;
  }
  else {
    valid_GPS = false;
  }
  
  gpsProcessPacket = false;
}

