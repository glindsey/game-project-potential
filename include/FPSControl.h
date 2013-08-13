#ifndef FPSCONTROL_H
#define FPSCONTROL_H

#include <boost/chrono.hpp>

#include "common.h"

class FrequencyControl
{
public:
  FrequencyControl();
  virtual ~FrequencyControl();

  /// Update the frequency counters.
  /// This function should be called at the start of each loop of the process you're measuring.
  /// It will update the frequency information.
  void loop();

  /// Get the instantaneous frequency.
  /// @return Frequency measured on the last loop.
  float getFrequency();

  /// Get the filtered frequency.
  /// Returns the frequency as measured via a low-pass filter algorithm.
  /// @return The filtered average frequency.
  float getFilteredFrequency();

private:
  // TODO: PIMPLize this
  float frequency;
  float filteredFrequency;

  boost::chrono::steady_clock::time_point lastCall;
};

#endif // CFPS_H
