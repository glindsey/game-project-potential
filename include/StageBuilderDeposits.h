/*
 * StageBuilderDeposits.h
 *
 *  Created on: Nov 30, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDERDEPOSITS_H_
#define STAGEBUILDERDEPOSITS_H_

#include "StageBuilder.h"

namespace gsl
{
namespace rectopia
{

/// This class generates the various mineral deposits inside the rock
/// strata.
class StageBuilderDeposits: public gsl::rectopia::StageBuilder
{
public:
  /// Create the mineral deposits on the stage.
  /// @param stage Reference to stage to build on.
  /// @param seed Random seed to use.
  StageBuilderDeposits(Stage& stage, int seed);
  virtual ~StageBuilderDeposits();

  /// Generate the mineral deposits.
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
#endif /* STAGEBUILDERDEPOSITS_H_ */
