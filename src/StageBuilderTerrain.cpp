// *** ADDED BY HEADER FIXUP ***
#include <iostream>
#include <vector>
// *** END ***
/*
 * StageBuilderTerrain.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: Gregory Lindsey
 */

#include "StageBuilderTerrain.h"

#include "Application.h"
#include "ColumnData.h"
#include "ErrorMacros.h"
#include "MathUtils.h"
#include "Stage.h"
#include "Settings.h"
#include "StageBlock.h"
#include "Substance.h"

#include <noise/noise.h>

namespace gsl
{
namespace rectopia
{

/// Typedef for a random distribution.
typedef boost::random::uniform_int_distribution<> RandomDistribution;

/// Typedef for a scoped pointer to a random distribution.
typedef std::unique_ptr<RandomDistribution> RDPointer;

enum class BuilderState
{
  Begin, BuildHeightMap, GenerateStrata, PopulateStage, Done
};

struct StageBuilderTerrain::Impl
{
  Impl(Stage& stage,
       int seed,
       StageCoord stage_height,
       StageCoord feature_height,
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

  /// State the builder is in.
  BuilderState builder_state_;

  /// Reference to the stage.
  Stage& stage_;

  /// Vector used to store strata info.
  std::vector<const Substance*> strata_;

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

  /// Minimum soil level.
  int soil_level_;

  /// Minimum sedimentary layer height.
  int sedimentary_level_;

  /// Minimum metamorphic layer height.
  int metamorphic_level_;

  /// Number of soil layer types.
  int soil_type_count_;

  /// Number of sedimentary layer types.
  int sedimentary_type_count_;

  /// Number of metamorphic layer types.
  int metamorphic_type_count_;

  /// Number of igneous layer types.
  int igneous_type_count_;

  /// Returns a random number within a given distribution.
  int GetRandom(RDPointer& distribution)
  {
    RandomDistribution& dist = *(distribution.get());
    return dist(App::instance().twister());
  }

  /// Random distribution for choosing sedimentary types.
  std::unique_ptr<RandomDistribution> sedimentary_distribution_;

  /// Random distribution for choosing metamorphic types.
  std::unique_ptr<RandomDistribution> metamorphic_distribution_;

  /// Random distribution for choosing igneous types.
  std::unique_ptr<RandomDistribution> igneous_distribution_;

  /// The column we are currently "painting".
  sf::Vector2i column_;

  bool begin_;
};

StageBuilderTerrain::StageBuilderTerrain(Stage& stage,
    int seed,
    StageCoord stage_height,
    StageCoord feature_height,
    double frequency,
    int octave_count,
    double persistence,
    double lacunarity)
  : impl(
      new Impl(stage, seed, stage_height, feature_height, frequency,
               octave_count, persistence, lacunarity))
{
  Reset();
}

StageBuilderTerrain::~StageBuilderTerrain()
{
  // TODO Auto-generated destructor stub
}

bool StageBuilderTerrain::Build()
{
  StageCoord3 stage_size = impl->stage_.size();

  if (impl->begin_)
  {
    impl->builder_state_ = BuilderState::Begin;
    impl->begin_ = false;
  }

  switch (impl->builder_state_)
  {
  case BuilderState::Begin:
  {
    impl->builder_state_ = BuilderState::BuildHeightMap;
    break;
  }

  case BuilderState::BuildHeightMap:
  {
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
    float perlin_seed = (impl->seed_ * 10) + 0.5f;
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
        int value = scale_bias.GetValue(perlin_x, perlin_y, perlin_seed);
        impl->stage_.setColumnInitialHeight(column.x, column.y, value);
      }
    }

    impl->builder_state_ = BuilderState::GenerateStrata;

    break;
  }

  case BuilderState::GenerateStrata:
  {
    std::cout << "Generating rock strata..." << std::endl;
    impl->strata_.clear();
    impl->strata_.resize(stage_size.z);

    impl->soil_level_ = 4;
    impl->sedimentary_level_ = ((float) impl->stage_height_ * 0.4f);
    impl->metamorphic_level_ = ((float) impl->stage_height_ * 0.7f);
    impl->soil_type_count_ = Substance::layerSoil.size();
    impl->sedimentary_type_count_ = Substance::layerSedimentary.size();
    impl->metamorphic_type_count_ = Substance::layerMetamorphic.size();
    impl->igneous_type_count_ = Substance::layerIgneousIntrusive.size();

    // Reset random distributions.
    impl->sedimentary_distribution_.reset(
      new RandomDistribution(0, impl->sedimentary_type_count_ - 1));
    impl->metamorphic_distribution_.reset(
      new RandomDistribution(0, impl->metamorphic_type_count_ - 1));
    impl->igneous_distribution_.reset(
      new RandomDistribution(0, impl->igneous_type_count_ - 1));

    // Handle the case if any of the layer vectors are empty!
    if (impl->sedimentary_type_count_ == 0)
    {
      FATAL_ERROR("No sedimentary layers were found for building terrain");
    }
    if (impl->metamorphic_type_count_ == 0)
    {
      FATAL_ERROR("No metamorphic layers were found for building terrain");
    }
    if (impl->igneous_type_count_ == 0)
    {
      FATAL_ERROR("No igneous layers were found for building terrain");
    }

    for (int z = 0; z < stage_size.z; z += 2)
    {
      const Substance* substance;

      if (z <= impl->soil_level_)
      {
        // TODO: Choose soil layer based on set conditions.
        substance = &(Substance::get("loam"));
      }
      else if (z <= impl->sedimentary_level_)
      {
        substance = Substance::layerSedimentary[impl->GetRandom(
            impl->sedimentary_distribution_)];
      }
      else if (z <= impl->metamorphic_level_)
      {
        substance = Substance::layerMetamorphic[impl->GetRandom(
            impl->metamorphic_distribution_)];
      }
      else
      {
        substance = Substance::layerIgneousIntrusive[impl->GetRandom(
                      impl->igneous_distribution_)];
      }

      impl->strata_[z] = substance;
      impl->strata_[z+1] = substance;
    }

    impl->column_ = sf::Vector2i(0, 0);
    std::cout << "Building initial stage...";
    impl->builder_state_ = BuilderState::PopulateStage;
    break;
  }

  case BuilderState::PopulateStage:
  {
    if (impl->column_.y < stage_size.y)
    {
      if (impl->column_.x < stage_size.x)
      {
        int stratum = 0;
        for (int z = impl->stage_.getColumnInitialHeight(impl->column_.x,
                                                         impl->column_.y) - 1;
             z >= 0; --z)
        {
          StageBlock& block = impl->stage_.get_block(impl->column_.x,
                                                    impl->column_.y,
                                                    z);

          const Substance* substance = impl->strata_[stratum];

          // This can be done "quickly" (no adjoining face invalidation)
          // since the stage is not yet designated "ready to render".
          block.set_substance_quickly(BlockLayer::Solid, *substance);
          ++stratum;
        }

        ++(impl->column_.x);
      }
      else
      {
        impl->column_.x = 0;
        std::cout << impl->column_.y << "... ";
        ++(impl->column_.y);
      }
    }
    else
    {
      std::cout << "done." << std::endl;
      impl->builder_state_ = BuilderState::Done;
    }

    break;
  }

  case BuilderState::Done:
  {
    return true;
  }
  }

  return false;
}

void StageBuilderTerrain::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
