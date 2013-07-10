/*
 * StageBuilderHeightMap.h
 *
 *  Created on: Nov 29, 2012
 *      Author: Gregory Lindsey
 */

#ifndef STAGEBUILDERHEIGHTMAP_H_
#define STAGEBUILDERHEIGHTMAP_H_

#include "StageBuilder.h"

namespace gsl
{
namespace rectopia
{

///
class StageBuilderHeightMap: public StageBuilder
{
public:
  /// Create the height map stage builder.
  /// @param stage Reference to stage to build on.
  /// @param seed Random seed to use.
  /// @param stage_height Average height of terrain features, in Z-levels.
  /// @param feature_height Total height of terrain features, in Z-levels.
  /// @param frequency Frequency of the Perlin noise used to generate the stage.
  /// @param octave_count Number of octaves of Perlin noise to use.
  /// @param persistence Perlin noise persistence (larger values = rougher terrain)
  /// @param lacunarity Perlin noise lacunarity (rather technical, look up Perlin noise for details)
  StageBuilderHeightMap(Stage& stage,
                        int seed,
                        StageCoord stage_height,
                        StageCoord feature_height,
                        double frequency,
                        int octave_count,
                        double persistence,
                        double lacunarity);
  virtual ~StageBuilderHeightMap();

  /// Generate the height map.
  /// As this is done quite quickly, it only takes one call.
  /// @return True, always.
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
#endif /* STAGEBUILDERHEIGHTMAP_H_ */
