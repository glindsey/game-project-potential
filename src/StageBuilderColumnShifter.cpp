// *** ADDED BY HEADER FIXUP ***
#include <algorithm>
#include <iostream>
// *** END ***
/*
 * StageBuilderColumnShifter.cpp
 *
 *  Created on: Nov 29, 2012
 *      Author: Gregory Lindsey
 */

#include "../include/StageBuilderColumnShifter.h"

#include "Application.h"
#include "ColumnData.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "Substance.h"

namespace gsl
{
namespace rectopia
{

/// Typedef for scoped pointer to random distribution.
typedef boost::random::uniform_int_distribution<> RandomDistribution;

struct StageBuilderColumnShifter::Impl
{
  Impl(Stage& stage, int seed, int bias)
    : stage_(stage), seed_(seed), bias_(bias)
  {
    Reset();
  }

  void Reset()
  {
    begin_ = true;
    coord_ = StageCoord2(0, 0);
  }

  void shiftColumn(StageCoord2 coord, StageCoord shiftAmount)
  {
    StageCoord lastChunkZ = stage_.size().z - 1;

    if (shiftAmount > 0)
    {
      // Shifting UP, so start at the top and work our way down.
      for (StageCoord n = lastChunkZ; n >= 0; n--)
      {
        StageCoord moveAmount = n - shiftAmount;

        StageBlock& newBlock = stage_.getBlock(coord.x, coord.y, n);
        StageBlock& oldBlock = stage_.getBlock(coord.x, coord.y,
                                               std::max((StageCoord)0, moveAmount));
        newBlock.set_substance(BlockLayer::Solid,
                               oldBlock.substance(BlockLayer::Solid));
        newBlock.set_substance(BlockLayer::Fluid,
                               oldBlock.substance(BlockLayer::Fluid));
      }
    }
    else if (shiftAmount < 0)
    {
      // Shifting DOWN, so start at the bottom and work up.
      for (StageCoord n = 0; n <= lastChunkZ; n++)
      {
        StageCoord moveAmount = n - shiftAmount;

        StageBlock& newBlock = stage_.getBlock(coord.x, coord.y, n);
        StageBlock& oldBlock = stage_.getBlock(coord.x, coord.y,
                                std::min(lastChunkZ, moveAmount));
        newBlock.set_substance(BlockLayer::Solid,
                               oldBlock.substance(BlockLayer::Solid));
        newBlock.set_substance(BlockLayer::Fluid,
                               oldBlock.substance(BlockLayer::Fluid));
      }
    }

    stage_.setColumnDirty(coord.x, coord.y);
  }

  /// Reference to the stage.
  Stage& stage_;

  /// Seed used for the RNG.
  int seed_;

  /// Bias value used to create the height map.
  /// (A fancy way of saying the average height of the map.)
  int bias_;

  /// Coordinates of the column being worked on.
  StageCoord2 coord_;

  bool begin_;
};

StageBuilderColumnShifter::StageBuilderColumnShifter(Stage& stage,
    int seed,
    int bias)
  : impl(new Impl(stage, seed, bias))
{
  // TODO Auto-generated constructor stub

}

StageBuilderColumnShifter::~StageBuilderColumnShifter()
{
  // TODO Auto-generated destructor stub
}

bool StageBuilderColumnShifter::Build()
{
  StageCoord3 stage_size = impl->stage_.size();

  if (impl->begin_)
  {
    std::cout << "Shifting columns..." << std::endl;
    impl->begin_ = false;
  }

  if (impl->coord_.x == 0)
  {
    std::cout << "    ... Row  " << impl->coord_.y << " ..." << std::endl;
  }

  if (impl->coord_.y < stage_size.y)
  {
    int column_height = impl->stage_.getColumnInitialHeight(impl->coord_.x,
                                                            impl->coord_.y);

    impl->shiftColumn(impl->coord_, column_height - impl->bias_);

    ++(impl->coord_.x);
    if (impl->coord_.x == stage_size.x)
    {
      impl->coord_.x = 0;
      ++(impl->coord_.y);
    }

    return false;
  }
  else
  {
    return true;
  }
}

void StageBuilderColumnShifter::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
