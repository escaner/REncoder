/*
 *   REncoder.cpp - Library to handle rotary encoders.
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

#include "REncoder.h"


const byte REncoder::_ST_MACHINE[][_NUM_COMBO] =
{
  // BA: { 00, 01, 10, 11 }
  // _ST_REST: 11
  // Invalid      CCW          CW           Same
  {  _ST_ERROR,   _ST_BGN_CCW, _ST_BGN_CW,  _ST_REST },
  // _ST_BGN_CW: 10
  // Fwd          Invalid      Same         Back
  {  _ST_MID_CW,  _ST_ERROR,   _ST_BGN_CW,  _ST_REST },
  // _ST_BGN_CCW: 01
  // Fwd          Same         Invalid      Back
  {  _ST_MID_CCW,  _ST_BGN_CCW, _ST_ERROR,   _ST_REST },
  // _ST_MID_CW: 00
  // Same         Fwd          Back         Invalid
  {  _ST_MID_CW,  _ST_END_CW,  _ST_BGN_CW,  _ST_ERROR },
  // _ST_MID_CCW: 00
  // Same         Back         Fwd          Invalid
  {  _ST_MID_CCW, _ST_BGN_CCW, _ST_END_CCW, _ST_ERROR },
  // _ST_END_CW: 01
  // Back         Same         Invalid      Fwd & CW
  {  _ST_MID_CW,  _ST_END_CW,  _ST_ERROR,   _ST_REST|_STEP_CW },
  // _ST_END_CCW: 10
  // Back         Invalid      Same         Fwd & CCW
  {  _ST_MID_CCW, _ST_ERROR,   _ST_REST,    _ST_REST|_STEP_CCW },
  // _ST_ERROR: come here when invalid state is detected
  // Invalid      Invalid      Invalid      Sync at Rest
  {  _ST_ERROR,   _ST_ERROR,   _ST_ERROR,   _ST_REST }
};


/*
 *   Constructor.
 *   Initializes state machine to neutral state. Assumes encoder in rest
 *  position.
 */
REncoder::REncoder()
{
  // Start State Machine from the encoder rest position
  _State = _ST_REST;
}


/*
 *   Updates the state machine with new encoder signals and returns whether
 *  a step has been completed.
 *   Parameters:
 *   * A (HIGH, LOW): value of A pin signal
 *   * B (HIGH, LOW): value of B pin signal
 *   Return:
 *   * 0: no step has been completed
 *   * 1: clockwise step has been completed
 *   * -1: counter clockwise step has been completed
 */
int8_t REncoder::update(uint8_t A, uint8_t B)
{
  byte Combo;

  // Get Combo for the next State
  Combo = _ST_MACHINE[_State][_packCode(A, B)];

  // Extract State from Combo
  _State = _comboGetState(Combo);

  // Extract Step from Combo and return it
  return _comboGetStep(Combo);
}


/*
 *   Given the bit values of A and B encoder signals, returns a Code
 *  containing both of them.
 *   Parameters:
 *   * A (HIGH, LOW): bit value for A pin
 *   * B (HIGH, LOW): bit value for B pin
 *   Return:
 *   * Byte with the encoded binary value of A and B in the following format:
 *     0b000000BA.
 */
inline byte REncoder::_packCode(uint8_t A, uint8_t B) const
{
  // Packs with B as high bit and A as low bit
  return (B << 1) | A;
}


/*
 *   Given a Combo value, returns a Step result.
 *   Parameters:
 *   * Combo: combo value with State and Step data
 *   Return:
 *   * -1: CCW step registered in the Combo
 *   *  0: No step registered in the Combo
 *   * +1: CW step registered in the Combo
 */
inline int8_t REncoder::_comboGetStep(byte Combo) const
{
  return ((int8_t) Combo) >> _COMBO_STEP_MASK_BIT;
}


/*
 *   Given a Combo value, returns a valid State.
 *   Parameters:
 *   * Combo: combo value with State and Step data
 *   Return:
 *   * ST_*: one of the valid defined States stored in the Combo.
 */
inline byte REncoder::_comboGetState(byte Combo) const
{
  return Combo & _COMBO_STATE_MASK;
}
