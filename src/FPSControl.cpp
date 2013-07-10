#include "FPSControl.h"

namespace gsl
{
namespace rectopia
{

FrequencyControl::FrequencyControl()
{
  frequency = 0;
  filteredFrequency = 0;
}

FrequencyControl::~FrequencyControl()
{
}

void FrequencyControl::loop()
{
  boost::chrono::steady_clock::time_point thisCall =
    boost::chrono::steady_clock::now();
  boost::chrono::duration<float> elapsed = thisCall - lastCall;

  frequency = 1.f / elapsed.count();

  filteredFrequency = ((filteredFrequency * 4.f) + frequency) / 5.f;

  lastCall = thisCall;
}

float FrequencyControl::getFrequency()
{
  return frequency;
}

float FrequencyControl::getFilteredFrequency()
{
  return filteredFrequency;
}

} // namespace rectopia
} // namespace gsl
