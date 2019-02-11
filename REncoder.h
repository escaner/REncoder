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
 * 
 *   The state of the encoder is followed by a state machine and updated
 *  through the update() method. When a full path is found from Rest state
 *  to the next State in one of the two directions, a Step event is returned.
 *   When an invalid code is detected (we missed at lease one code),
 *  the state machine goes to an Error state where it remains until a new
 *  Rest code is detected and it syncs there. All other codes are ignored
 *  until then.
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
  static const uint8_t ST_ERROR = 7;
  static const uint8_t ST_COUNT = 8;

  // Encoder Step identifiers with direction
  static const byte STEP_CW = 1 << 4;  // High half byte +1: 00010000
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Woverflow"
  static const byte STEP_CCW = ((byte) -1) << 4;  // High half byte -1: 11110000
  #pragma GCC diagnostic pop

  // Masks to split a Combo into State and Step
  static const byte COMBO_STATE_MASK = 0b00001111;
  static const byte COMBO_STEP_MASK = STEP_CW | STEP_CCW;  // 11110000

  // Number of combinations of the A & B lines
  static const uint8_t NUM_COMBO = 4;

  // State machine
  static const byte ST_MACHINE[][NUM_COMBO];

  // Current state in the State Machine
  uint8_t _State;

  // Methods
  inline byte _packCode(uint8_t A, uint8_t B) const;
  inline int8_t _comboGetStep(byte Combo) const;
  inline byte _comboGetState(byte Combo) const;
};

#endif  // _RENCODER_H_