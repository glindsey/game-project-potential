/*
 * StageBuilderSmoother.h
 *
 *  Created on: Dec 3, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDERSMOOTHER_H_
#define STAGEBUILDERSMOOTHER_H_

#include "StageBuilder.h"

namespace gsl
{
namespace rectopia
{

class StageBuilderSmoother: public gsl::rectopia::StageBuilder
{
public:
  /// Create the builder for smoothing the terrain surface.
  /// @param stage Reference to stage to build on.
  /// @param seed Random seed to use.
  StageBuilderSmoother(Stage& stage, int seed);
  virtual ~StageBuilderSmoother();

  /// Smooth the terrain surface.
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
#endif /* STAGEBUILDERSMOOTHER_H_ */
