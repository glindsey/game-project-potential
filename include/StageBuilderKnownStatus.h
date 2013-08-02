/*
 * StageBuilderKnownStatus.h
 *
 *  Created on: Dec 5, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDERKNOWNSTATUS_H_
#define STAGEBUILDERKNOWNSTATUS_H_

#include "StageBuilder.h"

namespace gsl
{
namespace rectopia
{

class StageBuilderKnownStatus: public gsl::rectopia::StageBuilder
{
public:
  /// Create the builder for setting the known status of blocks.
  /// @param stage Reference to stage to build on.
  /// @param seed Random seed to use.
  StageBuilderKnownStatus(Stage& stage, int seed);
  virtual ~StageBuilderKnownStatus();

  /// Set blocks' "known" status.
  /// @return False until finished; true when done.
  virtual bool Build();

  /// Reset the builder.
  virtual void Reset();

private:
  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};

} /* namespace rectopia */
} /* namespace gsl */
#endif /* STAGEBUILDERKNOWNSTATUS_H_ */
