/*
 * StageBuilderFlora.h
 *
 *  Created on: Dec 4, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDERFLORA_H_
#define STAGEBUILDERFLORA_H_

#include "StageBuilder.h"

class StageBuilderFlora: public StageBuilder
{
public:
  /// Create the builder for adding water features.
  /// @param stage Reference to stage to build on.
  /// @param seed Random seed to use.
  /// @param plains_threshold Threshold for open plains, from 0 to 100.
  /// @param forest_threshold Threshold for forests, from 0 to 100.
  StageBuilderFlora(Stage& stage,
                    int seed,
                    int plains_threshold,
                    int forest_threshold);
  virtual ~StageBuilderFlora();

  /// Add water features to the stage.
  /// @return False until finished; true when done.
  virtual bool Build();

  /// Reset the builder.
  virtual void Reset();

private:
  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};
#endif /* STAGEBUILDERFLORA_H_ */
