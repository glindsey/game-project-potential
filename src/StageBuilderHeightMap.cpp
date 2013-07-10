// *** ADDED BY HEADER FIXUP ***
#include <iostream>
// *** END ***
/*
 * StageBuilderHeightMap.cpp
 *
 *  Created on: Nov 29, 2012
 *      Author: Gregory Lindsey
 */

#include "StageBuilderHeightMap.h"

#include "ColumnData.h"
#include "MathUtils.h"
#include "Stage.h"

#include <noise/noise.h>

namespace gsl
{
namespace rectopia
{

struct StageBuilderHeightMap::Impl
{
  Impl(Stage& stage,
       int seed,
       int stage_height,
       int feature_height,
       double frequency,
       int octave_count,
       double persistence,
       double lacunarity)
    : stage_(stage),
      seed_(seed),
      stage_height_(stage_height),
      feature_height_(feature_height),
      frequency_(frequency),
      octave_count_(octave_count),
      persistence_(persistence),
      lacunarity_(lacunarity)
  {
    Reset();
  }

  void Reset()
  {
    begin_ = true;
  }

  /// Reference to the stage.
  Stage& stage_;

  /// Seed used for the RNG.
  int seed_;

  /// Average height of terrain features, in Z-levels.
  StageCoord stage_height_;

  /// Total height of terrain features, in Z-levels.
  StageCoord feature_height_;

  /// Frequency of Perlin noise.
  double frequency_;

  /// Octaves of Perlin noise to use.
  int octave_count_;

  /// Persistence of Perlin noise.
  double persistence_;

  /// Lacunarity of Perlin noise.
  double lacunarity_;

  bool begin_;
};

StageBuilderHeightMap::StageBuilderHeightMap(Stage& stage,
    int seed,
    StageCoord stage_height,
    StageCoord feature_height,
    double frequency,
    int octave_count,
    double persistence,
    double lacunarity)
  : StageBuilder(stage, seed),
    impl(
      new Impl(stage, seed, stage_height, feature_height, frequency,
               octave_count, persistence, lacunarity))
{
  Reset();
}

StageBuilderHeightMap::~StageBuilderHeightMap()
{
  // TODO Auto-generated destructor stub
}

bool StageBuilderHeightMap::Build()
{
  if (impl->begin_)
  {
    StageCoord3 stage_size = impl->stage_.size();

    std::cout << "Building height map for " << stage_size.x << " x "
              << stage_size.y << " x " << stage_size.z << " sized map..."
              << std::endl;

    std::cout << "DEBUG information: " << std::endl;
    std::cout << "  -- Seed = " << impl->seed_ << std::endl;
    std::cout << "  -- Frequency = " << impl->frequency_ << std::endl;
    std::cout << "  -- Octaves = " << impl->octave_count_ << std::endl;
    std::cout << "  -- Persistence = " << impl->persistence_ << std::endl;
    std::cout << "  -- Lacunarity = " << impl->lacunarity_ << std::endl;
    std::cout << "  -- Stage height (bias) = " << impl->stage_height_
              << std::endl;
    std::cout << "  -- Feature height (scale) = " << impl->feature_height_
              << std::endl;

    impl->begin_ = false;

    // Generate the required Perlin noise for the terrain.
    noise::module::Perlin perlin_noise;
    perlin_noise.SetSeed(impl->seed_);
    perlin_noise.SetNoiseQuality(noise::NoiseQuality::QUALITY_BEST);
    perlin_noise.SetFrequency(impl->frequency_);
    perlin_noise.SetOctaveCount(impl->octave_count_);
    perlin_noise.SetPersistence(impl->persistence_);
    perlin_noise.SetLacunarity(impl->lacunarity_);

    // Scale and bias the terrain.
    noise::module::ScaleBias scale_bias;
    scale_bias.SetSourceModule(0, perlin_noise);
    scale_bias.SetBias(impl->stage_height_);
    scale_bias.SetScale(impl->feature_height_);

    StageCoord2 column;
    for (column.x = 0; column.x < stage_size.x; ++column.x)
    {
      for (column.y = 0; column.y < stage_size.y; ++column.y)
      {
        double perlin_x = (double) column.x / (double) stage_size.x;
        double perlin_y = (double) column.y / (double) stage_size.y;
        int value = scale_bias.GetValue(perlin_x, perlin_y, 0.5f);
        //std::cout << "Setting column (" << column.x << ", " << column.y << ") to height " << value << std::endl;
        impl->stage_.setColumnInitialHeight(column.x, column.y, value);
      }
    }
  }

  return true;
}

void StageBuilderHeightMap::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
