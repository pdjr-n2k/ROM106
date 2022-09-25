/**********************************************************************
 * HexEncoder.h - recover a byte from a pair of rotary hex encoders.
 * 2022 (c) Paul Reeve.
 */

#ifndef HEXENCODER_H
#define HEXENCODER_H

#include <Arduino.h>

class HexEncoder {

public:
    HexEncoder(unsigned int gpioBit0, unsigned int gpioBit1, unsigned int gpioBit2, unsigned int gpioBit4, unsigned int gpioLo, unsigned int gpioHi);
    unsigned char getByte();

protected:

private:
    unsigned int gpioBit0;
    unsigned int gpioBit1;
    unsigned int gpioBit2;
    unsigned int gpioBit4;
    unsigned int gpioLo;
    unsigned int gpioHi;
    
};

#endif