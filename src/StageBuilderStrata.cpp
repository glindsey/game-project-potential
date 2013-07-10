// *** ADDED BY HEADER FIXUP ***
#include <iostream>
// *** END ***
/*
 * StageBuilderStrata.cpp
 *
 *  Created on: Nov 29, 2012
 *      Author: Gregory Lindsey
 */

#include "../include/StageBuilderStrata.h"

#include "Application.h"
#include "ErrorMacros.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "Substance.h"

namespace gsl
{
namespace rectopia
{

/// Typedef for a random distribution.
typedef boost::random::uniform_int_distribution<> RandomDistribution;

/// Typedef for a scoped pointer to a random distribution.
typedef boost::scoped_ptr<RandomDistribution> RDPointer;

struct StageBuilderStrata::Impl
{
  Impl(Stage& stage, int seed, int stage_height)
    : stage_(stage), seed_(seed), stage_height_(stage_height)
  {
    Reset();
  }

  void Reset()
  {
    soil_level_ = (stage_height_ - 4);
    sedimentary_level_ = ((float) stage_height_ * 0.8f);
    metamorphic_level_ = ((float) stage_height_ * 0.6f);
    soil_type_count_ = Substance::layerSoil.size();
    sedimentary_type_count_ = Substance::layerSedimentary.size();
    metamorphic_type_count_ = Substance::layerMetamorphic.size();
    igneous_type_count_ = Substance::layerIgneousIntrusive.size();
    y_ = 0;
    z_ = 0;

    // Reset random distributions.
    sedimentary_distribution_.reset(
      new RandomDistribution(0, sedimentary_type_count_ - 1));
    metamorphic_distribution_.reset(
      new RandomDistribution(0, metamorphic_type_count_ - 1));
    igneous_distribution_.reset(
      new RandomDistribution(0, igneous_type_count_ - 1));

    // Handle the case if any of the layer vectors are empty!
    if (sedimentary_type_count_ == 0)
    {
      FATAL_ERROR("No sedimentary layers were found to generate " << std::endl;
      Settings::handleFatalError(__FILE__, __LINE__);
    }
    if (metamorphic_type_count_ == 0)
    {
      std::cout << "FATAL: No metamorphic layers were found!" << std::endl;
      Settings::handleFatalError(__FILE__, __LINE__);
    }
    if (igneous_type_count_ == 0)
    {
      std::cout << "FATAL: No igneous layers were found!" << std::endl;
      Settings::handleFatalError(__FILE__, __LINE__);
    }

    begin_ = true;
  }

  /// Reference to the stage.
  Stage& stage_;

  /// Seed used for the RNG.
  int seed_;

  /// Total height of the terrain.
  StageCoord stage_height_;

  /// Minimum soil level.
  StageCoord soil_level_;

  /// Minimum sedimentary layer height.
  StageCoord sedimentary_level_;

  /// Minimum metamorphic layer height.
  StageCoord metamorphic_level_;

  /// Number of soil layer types.
  int soil_type_count_;

  /// Number of sedimentary layer types.
  int sedimentary_type_count_;

  /// Number of metamorphic layer types.
  int metamorphic_type_count_;

  /// Number of igneous layer types.
  int igneous_type_count_;

  /// Y-row we are currently on.
  StageCoord y_;

  /// Z-level we are currently on.
  StageCoord z_;

  /// Returns a random number within a given distribution.
  int GetRandom(RDPointer& distribution)
  {
    RandomDistribution& dist = *(distribution.get());
    return dist(App::instance().twister());
  }

  /// Random distribution for choosing sedimentary types.
  boost::scoped_ptr<RandomDistribution> sedimentary_distribution_;

  /// Random distribution for choosing metamorphic types.
  boost::scoped_ptr<RandomDistribution> metamorphic_distribution_;

  /// Random distribution for choosing igneous types.
  boost::scoped_ptr<RandomDistribution> igneous_distribution_;

  bool begin_;
};

StageBuilderStrata::StageBuilderStrata(Stage& stage, int seed, StageCoord stage_height)
  : StageBuilder(stage, seed), impl(new Impl(stage, seed, stage_height))
{
  // TODO Auto-generated constructor stub

}

StageBuilderStrata::~StageBuilderStrata()
{
  // TODO Auto-generated destructor stub
}

bool StageBuilderStrata::Build()
{
  static StageCoord3 stage_size = impl->stage_.size();
  static const Substance* stratumMaterial;

  if (impl->begin_)
  {
    std::cout << "Generating rock strata..." << std::endl;
    impl->begin_ = false;
  }

  if (impl->y_ == 0)
  {
    if (impl->z_ <= impl->stage_height_)
    {
      std::cout << "    ... Z-level " << impl->z_ << " ..." << std::endl;

      if (impl->z_ > impl->soil_level_)
      {
        // TODO: Choose soil layer based on set conditions.
        stratumMaterial = &(Substance::get("loam"));
      }
      else if (impl->z_ > impl->sedimentary_level_)
      {
        stratumMaterial = Substance::layerSedimentary[impl->GetRandom(
                            impl->sedimentary_distribution_)];
      }
      else if (impl->z_ > impl->metamorphic_level_)
      {
        stratumMaterial = Substance::layerMetamorphic[impl->GetRandom(
                            impl->metamorphic_distribution_)];
      }
      else
      {
        stratumMaterial = Substance::layerIgneousIntrusive[impl->GetRandom(
                            impl->igneous_distribution_)];
      }
    }
    else
    {
      return true;
    }
  }

  for (StageCoord x = 0; x < stage_size.x; ++x)
  {
    StageBlock& block = impl->stage_.getBlock(x, impl->y_, impl->z_);
    block.set_substance(BlockLayer::Solid, *stratumMaterial);
  }

  ++(impl->y_);
  if (impl->y_ == stage_size.y)
  {
    impl->y_ = 0;
    ++(impl->z_);
  }

  return false;
}

void StageBuilderStrata::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
