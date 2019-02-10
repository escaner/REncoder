/*
 *   REncoderAsync.cpp - Library to handle rotary encoders with async
 *  polling wait.
 *   Created by Escaner, 2019.
 */

#include "REncoderAsync.h"


/*
 *   Constructor.
 *   Initializes state machine to neutral state. Assumes encoder in rest
 *  position.
 */
REncoderAsync::REncoderAsync(uint8_t DelayPressRelease = DEFAULT_DELAY,
  uint8_t DelayReleasePress = DEFAULT_DELAY):
  REncoder(),
  _DelayPressRelease(DelayPressRelease),
  _DelayReleasePress(DelayReleasePress)
{
  _NumEvents = 0;
  _NextEventRelease = EV_NONE;
  _LastEventTime = //millis();
}

/*
 *   Updates the state machine with new encoder signals, keeps track of
 *  the number of events to send and returns whether a step has been completed.
 *   Effectively overwrites REncoder::update() by calling it and updating
 *  _NumEvents member.
 *   Parameters:
 *   * A (HIGH, LOW): value of A pin signal
 *   * B (HIGH, LOW): value of B pin signal
 *   Return:
 *   * 0: no step has been completed
 *   * 1: clockwise step has been completed
 *   * -1: counter clockwise step has been completed
 */
inline int8_t REncoderAsync::update(uint8_t A, uint8_t B)
{
  int8_t Step;

  // Update encoder status with superclass method
  Step = REncoder::update(A, B);

  // Keep count of the events in queue
  _NumEvents += Step;

  return Step;
}


/*
 *   Returns an identifier with the next event to be sent if the delay
 *  has passed and consumes it, or none otherwise,
 *  removing it from the queue.
 *   Returns: one of REncoderAsync::EV_*
 */
uint8_t REncoderAsync::getEvent()
{
  unsigned long CurrentTime;

  CurrentTime = millis();
}