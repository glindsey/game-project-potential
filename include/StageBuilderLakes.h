/*
 * StageBuilderLakes.h
 *
 *  Created on: Dec 10, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDERLAKES_H_
#define STAGEBUILDERLAKES_H_

#include "StageBuilder.h"

namespace gsl
{
namespace rectopia
{

class StageBuilderLakes: public gsl::rectopia::StageBuilder
{
public:
  /// Create the builder for adding lakes.
  /// @param stage Reference to stage to build on.
  /// @param seed Random seed to use.
  /// @param sea_level Sea level for lakes.
  StageBuilderLakes(Stage& stage, int seed, StageCoord sea_level);
  virtual ~StageBuilderLakes();

  /// Add water features to the stage.
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
#endif /* STAGEBUILDERLAKES_H_ */
