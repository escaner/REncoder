/*
 *   REncoder.h - Library to handle rotary encoders.
 *   Created by Escaner, 2019.
 */

#ifndef _RENCODER_H_
#define _RENCODER_H_

#include "Arduino.h"


/*
 *  Nomenclature:
 *  - CODE: Combination of A & B binary signals in format BA
 *  - STEP: Identifier indicating that a full click has happened in the encoder
 *          with its associated direction.
 *  - STATE: any of the several states in the state machine.
 *  - COMBO: byte that contains info on both STATE and STEP.
 *
 *  Rotary Encoder Gray Code (BA):
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
  int8_t update(uint8_t A, uint8_t B);

protected:
  // States for the state machine
  static const uint8_t ST_REST = 0;
  static const uint8_t ST_BGN_CW = 1;
  static const uint8_t ST_BGN_CCW = 2;
  static const uint8_t ST_MID_CW = 3;
  static const uint8_t ST_MID_CCW = 4;
  static const uint8_t ST_END_CW = 5;
  static const uint8_t ST_END_CCW = 6;
  static const uint8_t ST_COUNT = 7;

  // Encoder Step identifiers with direction
  static const byte STEP_CW = 1 << 4 ;   // High half byte +1: 0b00010000
  static const byte STEP_CCW = ((byte) -1) << 4;  // High half byte -1: 0b11110000

  // Masks to split a Combo into State and Step
  static const byte COMBO_STATE_MASK = 0b00001111;
  static const byte COMBO_STEP_MASK = STEP_CW | STEP_CCW;  // 0b11110000

  // Number of combinations of the A & B lines
  static const uint8_t NUM_COMBO = 4;

  // State machine
  static const byte ST_MACHINE[][NUM_COMBO];

  // Current state in the State Machine
  uint8_t State;

  // Methods
  inline byte packCode(uint8_t A, uint8_t B) const;
  inline int8_t comboGetStep(byte Combo) const;
  inline byte comboGetState(byte Combo) const;
};

#endif  // _RENCODER_H_