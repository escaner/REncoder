/*
 *   REncoder.cpp - Library to handle rotary encoders.
 *   Created by Escaner, 2019.
 */

#include "REncoder.h"


const byte REncoder::ST_MACHINE[][NUM_COMBO] =
{
  // BA: { 00, 01, 10, 11 }
  // ST_REST: 11
  // Invalid     CCW         CW          Same
  {  ST_REST,    ST_BGN_CCW, ST_BGN_CW,  ST_REST },
  // ST_BGN_CW: 10
  // Fwd         Invalid     Same        Back
  {  ST_MID_CW,  ST_REST,    ST_BGN_CW,  ST_REST },
  // ST_BGN_CCW: 01
  // Fwd         Same        Invalid     Back
  {  ST_MID_CCW, ST_BGN_CCW, ST_REST,    ST_REST },
  // ST_MID_CW: 00
  // Same        Fwd         Back        Invalid
  {  ST_MID_CW,  ST_END_CW,  ST_BGN_CW,  ST_REST },
  // ST_MID_CCW: 00
  // Same        Back        Fwd         Invalid
  {  ST_MID_CCW, ST_BGN_CCW, ST_END_CCW, ST_REST },
  // ST_END_CW: 01
  // Back        Same        Invalid     Fwd & CW
  {  ST_MID_CW,  ST_END_CW,  ST_REST,    ST_REST|STEP_CW },
  // ST_END_CCW: 10
  // Back        Invalid     Same        Fwd & CCW
  {  ST_MID_CCW, ST_REST,    ST_REST,    ST_REST|STEP_CCW }
};


/*
 *   Constructor.
 *   Initializes state machine to neutral state. Assumes encoder in rest
 *  position.
 */
REncoder::REncoder()
{
  // Start State Machine from the encoder rest position
  State = ST_REST;
}


/*
 *   Updates the state machine with new encoder signals and returns whether
 *  a step has been completed.
 *   Parameters:
 *   * A: value of A pin signal
 *   * B: value of B pin signal
 *   Return:
 *   * 0: no step has been completed
 *   * 1: clockwise step has been completed
 *   * -1: counter clockwise step has been completed
 */
int8_t REncoder::update(bool A, bool B)
{
  byte Combo;

  // Get Combo for the next State
  Combo = ST_MACHINE[State][packCode(A, B)];

  // Extract State from Combo
  State = comboGetState(Combo);

  // Extract Step from Combo and return it
  return comboGetStep(Combo);
}


/*
 *   Given the bit values of A and B encoder signals, returns a Code
 *  containing both of them.
 *   Parameters:
 *   * A: bit value for A pin
 *   * B: bit value for B pin
 *   Return:
 *   * Byte with the encoded binary value of A and B in the following format:
 *     0b000000BA, where true is 1 and false is 0.
 */
inline byte REncoder::packCode(bool A, bool B) const
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
inline int8_t REncoder::comboGetStep(byte Combo) const
{
  return ((int8_t) Combo) >> 4;
}


/*
 *   Given a Combo value, returns a valid State.
 *   Parameters:
 *   * Combo: combo value with State and Step data
 *   Return:
 *   * ST_*: one of the valid defined States stored in the Combo.
 */
inline byte REncoder::comboGetState(byte Combo) const
{
  return Combo & COMBO_STATE_MASK;
}
