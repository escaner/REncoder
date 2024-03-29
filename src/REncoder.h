/*
 *   REncoder.h - Library to handle rotary encoders.
 *
 *   Copyright (C) 2019 Óscar Laborda
 *
 *   This file is part of REncoder library.
 *
 *   REncoder is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *   REncoder is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *  along with REncoder.  If not, see <https://www.gnu.org/licenses/>.
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
  static const uint8_t _ST_REST = 0;
  static const uint8_t _ST_BGN_CW = 1;
  static const uint8_t _ST_BGN_CCW = 2;
  static const uint8_t _ST_MID_CW = 3;
  static const uint8_t _ST_MID_CCW = 4;
  static const uint8_t _ST_END_CW = 5;
  static const uint8_t _ST_END_CCW = 6;
  static const uint8_t _ST_ERROR = 7;
  // static const uint8_t _ST_COUNT = 8;

  // Encoder Step identifiers with direction
  static const byte _STEP_CW = 1 << 4;  // High half byte +1: 00010000
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Woverflow"
  static const byte _STEP_CCW = ((byte) -1) << 4; // High half byte -1: 11110000
  #pragma GCC diagnostic pop

  // Masks to split a Combo into State
  static const byte _COMBO_STATE_MASK = 0b00001111;
  // Step Mask starts at bit 4: 11110000
  static const uint8_t _COMBO_STEP_MASK_BIT = 4;  

  // Number of combinations of the A & B lines
  static const uint8_t _NUM_COMBO = 4;

  // State machine
  static const byte _ST_MACHINE[][_NUM_COMBO];

  // Current state in the State Machine
  uint8_t _State;

  // Methods
  inline byte _packCode(uint8_t A, uint8_t B) const;
  inline int8_t _comboGetStep(byte Combo) const;
  inline byte _comboGetState(byte Combo) const;
};

#endif  // _RENCODER_H_