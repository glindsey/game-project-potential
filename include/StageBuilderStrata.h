/*
 * StageBuilderStrata.h
 *
 *  Created on: Nov 29, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDERSTRATA_H_
#define STAGEBUILDERSTRATA_H_

#include "StageBuilder.h"

namespace gsl
{
namespace rectopia
{

class StageBuilderStrata: public gsl::rectopia::StageBuilder
{
public:
  /// Create the rock strata on the stage.
  /// @param stage Reference to stage to build on.
  /// @param seed Random seed to use.
  /// @param stage_height Total height of terrain, in Z-levels.
  StageBuilderStrata(Stage& stage, int seed, StageCoord stage_height);
  virtual ~StageBuilderStrata();

  /// Generate the rock strata.
  /// @return False until finished; true when done.
  virtual bool Build();

  /// Reset the builder.
  virtual void Reset();

private:
  struct Impl;
  /// Private implementation pointer
  boost::scoped_ptr<Impl> impl;
};

} /* namespace rectopia */
} /* namespace gsl */
#endif /* STAGEBUILDERSTRATA_H_ */
