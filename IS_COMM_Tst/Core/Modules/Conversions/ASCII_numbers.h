// ASCII_numbers.h


#ifndef _ASCII_NUMBERS_H_
#define _ASCII_NUMBERS_H_


// Includes
#include "main.h"
#include <stdint.h>


// typedefs, structs, and enums



// defines



// private modular variables



// public variables



// public function prototypes
uint8_t ConvertASCII2UINT16(const char * numstring, uint8_t _len, const char _terminator, volatile uint16_t * value);
uint8_t GPSConvertASCII2FLOAT(const char * numstring, uint8_t _len, volatile float * _number);
uint8_t GPSConvertASCII2UINT16(const char * numstring, uint8_t _len, volatile uint16_t * _number);
uint8_t GPSConvertHEX2DEC(const char * numstring, uint8_t _len, volatile uint16_t * _number);


#endif
