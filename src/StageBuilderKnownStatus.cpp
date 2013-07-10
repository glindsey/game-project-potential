// *** ADDED BY HEADER FIXUP ***
#include <iostream>
// *** END ***
/*
 * StageBuilderKnownStatus.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: Gregory Lindsey
 */

#include "../include/StageBuilderKnownStatus.h"

#include "Application.h"
#include "BlockTopCorners.h"
#include "ColumnData.h"
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

struct StageBuilderKnownStatus::Impl
{
  Impl(Stage& stage, int seed)
    : stage_(stage), seed_(seed)
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
};

StageBuilderKnownStatus::StageBuilderKnownStatus(Stage& stage, int seed)
  : impl(new Impl(stage, seed))
{
}

StageBuilderKnownStatus::~StageBuilderKnownStatus()
{
}

bool StageBuilderKnownStatus::Build()
{
  static StageCoord3 stage_size = impl->stage_.size();
  StageCoord3 coord3d;

  if (impl->begin_)
  {
    std::cout << "Setting \"known\" status of blocks..." << std::endl;

    impl->column_ = StageCoord2(0, 0);
    impl->begin_ = false;
  }

  if (impl->column_.y < stage_size.y)
  {
    for (impl->column_.x = 0; impl->column_.x < stage_size.x;
         ++(impl->column_.x))
    {
      coord3d.x = impl->column_.x;
      coord3d.y = impl->column_.y;

      for (coord3d.z = stage_size.z - 1;
           coord3d.z >= impl->stage_.getColumnOutdoorHeight(coord3d.x,
                                                            coord3d.y);
           coord3d.z--)
      {
        StageBlock& block = impl->stage_.getBlock(coord3d.x, coord3d.y, coord3d.z);
        block.setKnown(true);
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

void StageBuilderKnownStatus::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
