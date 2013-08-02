// *** ADDED BY HEADER FIXUP ***
#include <algorithm>
#include <iostream>
// *** END ***
/*
 * StageBuilderSmoother.cpp
 *
 *  Created on: Dec 3, 2012
 *      Author: Gregory Lindsey
 */

#include "../include/StageBuilderSmoother.h"

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

struct StageBuilderSmoother::Impl
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

  /// Coordinate we are currently working on.
  StageCoord2 coord_;

  /// Reference to the stage.
  Stage& stage_;

  /// Seed used for the RNG.
  int seed_;
};

StageBuilderSmoother::StageBuilderSmoother(Stage& stage, int seed)
  : impl(new Impl(stage, seed))
{
  // TODO Auto-generated constructor stub

}

StageBuilderSmoother::~StageBuilderSmoother()
{
  // TODO Auto-generated destructor stub
}

bool StageBuilderSmoother::Build()
{
  static StageCoord3 stage_size = impl->stage_.size();

  if (impl->begin_)
  {
    std::cout << "Smoothing terrain..." << std::endl;
    impl->coord_ = StageCoord2(0, 0);
    impl->begin_ = false;
  }

  if (impl->coord_.y != stage_size.y)
  {
    for (impl->coord_.x = 0; impl->coord_.x < stage_size.x; ++(impl->coord_.x))
    {
      int height = impl->stage_.getColumnSolidHeight(impl->coord_.x,
                                                     impl->coord_.y);

      StageBlock& block = impl->stage_.get_block(impl->coord_.x,
                                                impl->coord_.y,
                                                height - 1);

      int x_left = std::max(0, impl->coord_.x - 1);
      int x_right = std::min(impl->coord_.x + 1, stage_size.x - 1);
      int y_up = std::max(0, impl->coord_.y - 1);
      int y_down = std::min(impl->coord_.y + 1, stage_size.y - 1);

      int heightN = impl->stage_.getColumnSolidHeight(impl->coord_.x, y_up);
      int heightNE = impl->stage_.getColumnSolidHeight(x_right, y_up);
      int heightE = impl->stage_.getColumnSolidHeight(x_right, impl->coord_.y);
      int heightSE = impl->stage_.getColumnSolidHeight(x_right, y_down);
      int heightS = impl->stage_.getColumnSolidHeight(impl->coord_.x, y_down);
      int heightSW = impl->stage_.getColumnSolidHeight(x_left, y_down);
      int heightW = impl->stage_.getColumnSolidHeight(x_left, impl->coord_.y);
      int heightNW = impl->stage_.getColumnSolidHeight(x_left, y_up);

      block.invalidate_face_data();
    }

    ++(impl->coord_.y);
    return false;
  }
  else
  {
    // We're done.
    return true;
  }
}

void StageBuilderSmoother::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
