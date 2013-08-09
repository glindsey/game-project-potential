// *** ADDED BY HEADER FIXUP ***
#include <iostream>
// *** END ***
/*
 * StageBuilderLakes.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: Gregory Lindsey
 */

#include "../include/StageBuilderLakes.h"

#include "Application.h"
#include "ColumnData.h"
#include "MathUtils.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "StageChunk.h"
#include "Substance.h"

namespace gsl
{
namespace rectopia
{

struct StageBuilderLakes::Impl
{
  Impl(Stage& stage, int seed, StageCoord sea_level)
    : stage_(stage), seed_(seed), sea_level_(sea_level)
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

  /// Sea level for the state.
  StageCoord sea_level_;
};

StageBuilderLakes::StageBuilderLakes(Stage& stage, int seed, StageCoord sea_level)
  : impl(new Impl(stage, seed, sea_level))
{
}

StageBuilderLakes::~StageBuilderLakes()
{
}

bool StageBuilderLakes::Build()
{
  static StageCoord3 stage_size = impl->stage_.size();

  if (impl->begin_)
  {
    std::cout << "Adding lakes..." << std::endl;

    impl->column_ = StageCoord2(0, 0);
    impl->begin_ = false;
  }

  if (impl->column_.y < stage_size.y)
  {
    for (impl->column_.x = 0; impl->column_.x < stage_size.x;
         ++(impl->column_.x))
    {
      StageCoord min_terrain_height = impl->stage_.min_terrain_height();

      for (StageCoord z = min_terrain_height + impl->sea_level_; z >= 0; --z)
      {
        StageCoord3 coord(impl->column_.x, impl->column_.y, z);
        StageBlock& block = impl->stage_.get_block(coord.x, coord.y, coord.z);

        if (block.is_traversable())
        {
          block.set_substance(BlockLayer::Fluid, Substance::get("freshwater"));
        }
        else
        {
          break;
        }
      }
      // Height, outdoor level need recalculating after this.
      impl->stage_.set_column_dirty(impl->column_.x, impl->column_.y);
    }

    ++(impl->column_.y);
  }
  else
  {
    return true;
  }

  return false;
}

void StageBuilderLakes::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
