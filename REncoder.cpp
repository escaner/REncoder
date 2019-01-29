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


REncoder::REncoder()
{
  // Start State Machine from the encoder rest position
  State = ST_REST;
}

uint8_t REncoder::update(bool A, bool B)
{
  
}