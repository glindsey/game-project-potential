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
#include "ColumnData.h"
#include "MathUtils.h"
#include "NoiseField.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "StageChunk.h"
#include "Substance.h"

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

  /// Z-level we're currently at.
  int z_level_;

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
  /// @todo This works for now, but what we REALLY need is basically a 3-D
  ///       flood-fill that starts at the top center of the stage and flows
  ///       down until it hits opaque blocks.

  static StageCoord3 stage_size = impl->stage_.size();
  StageCoord3 coord3d;

  if (impl->begin_)
  {
    std::cout << "Setting initial \"known\" status of blocks...";

    impl->z_level_ = stage_size.z;
    impl->begin_ = false;
  }

  std::cout << impl->z_level_ << "...";

  if (impl->z_level_ != 0)
  {
    --(impl->z_level_);
    StageCoord2 coord(0, 0);
    for (coord.x = 0; coord.x < stage_size.x; ++coord.x)
    {
      for (coord.y = 0; coord.y < stage_size.y; ++coord.y)
      {
        StageBlock& block = impl->stage_.get_block(coord.x,
                                                   coord.y,
                                                   impl->z_level_);

        if (impl->z_level_ == stage_size.z - 1)
        {
          block.set_known(true);
        }
        else
        {
          StageBlock& block_above = impl->stage_.get_block(coord.x,
                                                           coord.y,
                                                           impl->z_level_ + 1);
          if (block_above.is_known() && !(block_above.is_opaque()))
          {
            block.set_known(true);
          }
        }
      }
    }
  }
  else
  {
    std::cout << "done." << std::endl;
    return true;
  }

  return false;
}

void StageBuilderKnownStatus::Reset()
{
  impl->Reset();
}
