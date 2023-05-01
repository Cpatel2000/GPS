// Number Conversions
// ASCII_number.c
//


// includes
#include "ASCII_numbers.h"


// modular variables


// external variables



// Code
uint8_t ConvertASCII2UINT16(const char * numstring, uint8_t _len, const char _terminator, volatile uint16_t * _number)
{
  uint8_t i;
  uint8_t error;
  uint8_t done;
  uint16_t value;
  
  
  error = 0;
  i = 0;
  value = 0;
  done = false;
  
  while ((i< _len) && (done != true) && (error == 0)) {
    if ((numstring[i] >= '0') && (numstring[i] <= '9') || (numstring[i] == ' ')) { 
      if (numstring[i] != ' ') {
        value *=10;
        value += (numstring[i] - 0x30);
      }
      i++;
      if ((numstring[i] == _terminator) || (numstring[i] == 0)) {
        done = true;
      }
    }
    else {
      error = 1;
    }
  }
  
  if (error == 0) {
    *_number = value;
  }
  
  return error;
}

uint8_t GPSConvertASCII2FLOAT(const char * numstring, uint8_t _len, volatile float * _number)
{
  uint8_t i;
  uint8_t j;
  uint8_t error;
  uint8_t done;
  //uint16_t value;
  int32_t value;
  uint8_t num_of_chars_after_dp;
  uint8_t start_dp_count;
  int32_t multiplier;
  uint8_t isNegative;
  
  
  error = 0;
  i = 0;
  j = 0;
  value = 0;
  done = false;
  multiplier = 1;
  isNegative = 0;
  
  num_of_chars_after_dp = 0;
  start_dp_count = 0;
  
  while ((i< _len) && (done != true) && (error == 0)) {
    if ((numstring[i] >= '0') && (numstring[i] <= '9') || (numstring[i] == ' ')) { 
      if (start_dp_count == 1) {
        num_of_chars_after_dp++;
      }
      if (numstring[i] != ' ') {
        value *=10;
        if (!isNegative) value += (numstring[i] - 0x30);
        if (isNegative) value -= (numstring[i] - 0x30);
      }
      i++;
      //if ((numstring[i] == _terminator) || (numstring[i] == 0)) {
      if ((i >= _len) || (numstring[i] == 0)) {
        done = true;
      }
    }
    else if (numstring[i] == '.') {
      start_dp_count = 1;
      i++;
    }
    else if (numstring[0] == '-') {
      isNegative = 1;
      i++;
    }
    else {
      error = 1;
    }
  }
  
  if (error == 0) {
    for (j = 0; j < num_of_chars_after_dp; j++) {
      multiplier *= 10;
    }
    *_number = (float) value / (float)multiplier;
  }
  
  return error;
}



uint8_t GPSConvertASCII2UINT16(const char * numstring, uint8_t _len, volatile uint16_t * _number)
{
  uint8_t i;
  uint8_t error;
  uint8_t done;
  uint16_t value;
  
  
  error = 0;
  i = 0;
  value = 0;
  done = false;
  
  while ((i< _len) && (done != true) && (error == 0)) {
    if ((numstring[i] >= '0') && (numstring[i] <= '9') || (numstring[i] == ' ')) { 
      if (numstring[i] != ' ') {
        value *=10;
        value += (numstring[i] - 0x30);
      }
      i++;
      if ((i >= _len) || (numstring[i] == 0)) {
        done = true;
      }
    }
    else {
      error = 1;
    }
  }
  
  if (error == 0) {
    *_number = value;
  }
  
  return error;
}

uint8_t GPSConvertHEX2DEC(const char * numstring, uint8_t _len, volatile uint16_t * _number)
{
  uint8_t i;
  uint8_t error;
  uint8_t done;
  uint16_t value;
  
  
  error = 0;
  i = 0;
  value = 0;
  done = false;
  
  while ((i< _len) && (done != true) && (error == 0)) {
    if (((numstring[i] >= '0') && (numstring[i] <= '9')) || ((numstring[i] >= 'A') && (numstring[i] <= 'F')) || (numstring[i] == ' ')) { 
      if (numstring[i] != ' ') {
        value *=16;
        if ((numstring[i] >= '0') && (numstring[i] <= '9')) value += (numstring[i] - '0');
        if ((numstring[i] >= 'A') && (numstring[i] <= 'F')) value += (numstring[i] - 'A');
      }
      i++;
      if ((i >= _len) || (numstring[i] == 0)) {
        done = true;
      }
    }
    else {
      error = 1;
    }
  }
  
  if (error == 0) {
    *_number = value;
  }
  
  return error;
}