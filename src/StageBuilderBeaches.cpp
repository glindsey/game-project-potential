// *** ADDED BY HEADER FIXUP ***
#include <iostream>
// *** END ***
/*
 * StageBuilderBeaches.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: Gregory Lindsey
 */

#include "StageBuilderBeaches.h"

#include "Application.h"
#include "BlockTopCorners.h"
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

struct StageBuilderBeaches::Impl
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

StageBuilderBeaches::StageBuilderBeaches(Stage& stage, int seed, StageCoord sea_level)
  : impl(new Impl(stage, seed, sea_level))
{
}

StageBuilderBeaches::~StageBuilderBeaches()
{
}

bool StageBuilderBeaches::Build()
{
  static StageCoord3 stage_size = impl->stage_.size();

  if (impl->begin_)
  {
    std::cout << "Adding beaches..." << std::endl;

    impl->column_ = StageCoord2(0, 0);
    impl->begin_ = false;
  }

  if (impl->column_.y < stage_size.y)
  {
    for (impl->column_.x = 0; impl->column_.x < stage_size.x;
         ++(impl->column_.x))
    {
      int min_terrain_height = impl->stage_.min_terrain_height();

      for (int z = min_terrain_height + impl->sea_level_;
           z >= min_terrain_height + impl->sea_level_ - 1; --z)
      {
        StageBlock& block = impl->stage_.getBlock(impl->column_.x, impl->column_.y, z);
        StageBlock& block_above = impl->stage_.getBlock(impl->column_.x, impl->column_.y, z + 1);

        if (block.solid() && !block_above.solid())
        {
          block.setSubstance(BlockLayer::Solid, Substance::get("sand"));
        }
      }

      // Height, outdoor level need recalculating after this.
      impl->stage_.setColumnDirty(impl->column_.x, impl->column_.y);
    }

    ++(impl->column_.y);
  }
  else
  {
    return true;
  }

  return false;
}

void StageBuilderBeaches::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
