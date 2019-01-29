/*****************/
typedef unsigned char byte;
/*****************/
/*
 *   REncoder.h - Library to handle rotary encoders.
 *   Created by Escaner, 2019.
 */

#ifndef _RENCODER_H_
#define _RENCODER_H_

#include "Arduino.h"


/*
 *  Rotary Encoder.
 *  Gray Code (BA):
 *         CW  CCW
 *   Rest  11   11
 *   Bgn   10   01
 *   Mid   00   00
 *   End   01   10
 *   Rest  11   11
 */

class REncoder
{
public:
  REncoder();
  int update(bool A, bool B);

protected:
  static const uint8_t ST_REST = 0;
  static const uint8_t ST_BGN_CW = 1;
  static const uint8_t ST_BGN_CCW = 2;
  static const uint8_t ST_MID_CW = 3;
  static const uint8_t ST_MID_CCW = 4;
  static const uint8_t ST_END_CW = 5;
  static const uint8_t ST_END_CCW = 6;
  static const uint8_t ST_COUNT = 7;

  static const byte STEP_CW =  0b00010000;
  static const byte STEP_CCW = 0b00100000;
  static const byte STEP_MASK = STEP_CW | STEP_CCW;  // 0b00110000
  static const byte CODE_A_MASK = 0b00000001;
  static const byte CODE_B_MASK = 0b00000010;

  // Number of combinations of the A & B lines
  static const uint8_t NUM_COMBO = 4;

  // State machine
  static const byte ST_MACHINE[][NUM_COMBO];

  // Current state in the State Machine
  uint8_t State;

  inline byte joinCode(bool A, bool B) const __attribute__((always_inline));
  inline void splitCode(byte Code, bool &A, bool &B) const __attribute__((always_inline));
  inline byte getStep(byte Code) const __attribute__((always_inline));
  inline byte getCode(byte Code) const __attribute__((always_inline));
};

#endif