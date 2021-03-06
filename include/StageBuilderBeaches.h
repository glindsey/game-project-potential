/*
 * StageBuilderBeaches.h
 *
 *  Created on: Dec 10, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDERBEACHES_H_
#define STAGEBUILDERBEACHES_H_

#include "StageBuilder.h"

class StageBuilderBeaches: public StageBuilder
{
public:
  /// Create the builder for adding beaches.
  /// @param stage Reference to stage to build on.
  /// @param seed Random seed to use.
  StageBuilderBeaches(Stage& stage, int seed, StageCoord sea_level);
  virtual ~StageBuilderBeaches();

  /// Add beaches to the stage.
  /// @return False until finished; true when done.
  virtual bool Build();

  /// Reset the builder.
  virtual void Reset();

private:
  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};

#endif /* STAGEBUILDERBEACHES_H_ */
