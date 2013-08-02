/*
 * StageBuilderRivers.h
 *
 *  Created on: Dec 10, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDERRIVERS_H_
#define STAGEBUILDERRIVERS_H_

#include "StageBuilder.h"

namespace gsl
{
namespace rectopia
{

class StageBuilderRivers: public gsl::rectopia::StageBuilder
{
public:
  /// Create the builder for adding rivers.
  /// @param stage Reference to stage to build on.
  /// @param seed Random seed to use.
  /// @param sea_level Sea level for water features.
  StageBuilderRivers(Stage& stage, int seed, StageCoord sea_level);
  virtual ~StageBuilderRivers();

  /// Add rivers to the stage.
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
#endif /* STAGEBUILDERRIVERS_H_ */
