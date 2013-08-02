// *** ADDED BY HEADER FIXUP ***
#include <iostream>
// *** END ***
/*
 * StageBuilderFlora.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: Gregory Lindsey
 */

#include "../include/StageBuilderFlora.h"

#include "Application.h"
#include "ColumnData.h"
#include "ErrorMacros.h"
#include "MathUtils.h"
#include "NoiseField.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "StageChunk.h"
#include "Substance.h"

namespace gsl
{
namespace rectopia
{

struct StageBuilderFlora::Impl
{
  Impl(Stage& stage, int seed, int plains_threshold, int forest_threshold)
    : stage_(stage),
      seed_(seed),
      plains_threshold_(plains_threshold),
      forest_threshold_(forest_threshold)
  {
    Reset();
  }

  void Reset()
  {

    begin_ = true;
  }

  bool begin_;

  /// Column we're currently at.
  StageCoord2 column_;

  /// Reference to the stage.
  Stage& stage_;

  /// Seed used for the RNG.
  int seed_;

  /// Threshold for noise field generating plains.
  StageCoord plains_threshold_;

  /// Threshold for noise field generating forests.
  StageCoord forest_threshold_;
};

StageBuilderFlora::StageBuilderFlora(Stage& stage,
                                     int seed,
                                     int plains_threshold,
                                     int forest_threshold)
  : impl(new Impl(stage, seed, plains_threshold, forest_threshold))
{
}

StageBuilderFlora::~StageBuilderFlora()
{
}

bool StageBuilderFlora::Build()
{
  static StageCoord3 stage_size = impl->stage_.size();
  SerialNumber prop_number;

  // TODO: magic numbers, u haz them
  static NoiseField forest_noisefield(stage_size.x, 0, 100, 8192, impl->seed_);
  static boost::random::uniform_int_distribution<> forest_distribution(
    impl->plains_threshold_, impl->forest_threshold_);

  if (impl->begin_)
  {
    std::cout << "Adding flora..." << std::endl;

    impl->column_ = StageCoord2(0, 0);
    impl->begin_ = false;
  }

  if (impl->column_.y < stage_size.y)
  {

    for (impl->column_.x = 0; impl->column_.x < stage_size.x;
         ++(impl->column_.x))
    {
      int height = impl->stage_.getColumnSolidHeight(impl->column_.x,
                                                     impl->column_.y);

      if (height < 0)
      {
        MINOR_ERROR("Column height %d for column (%u, %u) is out of bounds",
                    height, impl->column_.x, impl->column_.y);
        height = 0;
      }

      if (height >= stage_size.z)
      {
        MINOR_ERROR("Column height %d for column (%u, %u) is out of bounds",
                    height, impl->column_.x, impl->column_.y);
        height = stage_size.z - 1;
      }

      // Figure out if the chunk at this location is soil.
      StageBlock& block = impl->stage_.get_block(impl->column_.x,
                                                impl->column_.y,
                                                height - 1);
      StageBlock& block_above = impl->stage_.get_block(impl->column_.x,
                                                      impl->column_.y,
                                                      height);

      bool block_is_soil =
        block.get_substance(BlockLayer::Solid).getProperties().get<bool>(
          "attributes.soil", false);
      /*
       bool thisChunkIsWater =
       thisNode.get_block()->getSubstance(BlockLayer::Fluid)->getProperties().get<bool>(
       "attributes.water", false);
       */
      bool above_is_water =
        block_above.get_substance(BlockLayer::Fluid).getProperties().get<bool>(
          "attributes.water", false);

      // Make sure this chunk is soil.
      if (block_is_soil)
      {
        // Above-water plants...
        if (!above_is_water)
        {
          // First put down grass.
          // TODO: Smatterings of different grass types.
          prop_number = Prop::create("grass");
          Prop::get(prop_number).move_to(block.get_inventory());

          // TODO: Bushes, weeds.

          // Create a forest distribution.
          // TODO: It might be better to generate simple Perlin noise between
          //       0 and 1, and then apply the following function:
          // chance = (arctan((value - bias) * sharpness) * (1 / pi)) + 0.5
          // where:
          //     value = value from the Perlin noise
          //    chance = chance of a tree on the square (between 0 and 1)
          //      bias = bias toward forest or plains (0 = forest, 1 = plains)
          // sharpness = sharpness of transition (20 is a good middle)
          int chance = forest_distribution(App::instance().twister());
          int value = (forest_noisefield.getScaledValue(impl->column_.x,
                       impl->column_.y));

          // Check the tree noisefield, put down trees if appropriate.
          // TODO: different kinds of trees
          if (value >= chance)
          {
            prop_number = Prop::create("tree");
            Prop::get(prop_number).move_to(block.get_inventory());
          }
        }
      }
    }

    ++(impl->column_.y);

  }
  else
  {
    return true;
  }

  return false;
}

void StageBuilderFlora::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
