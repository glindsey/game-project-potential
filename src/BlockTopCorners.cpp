/*
 * BlockTopCorners.cpp
 *
 *  Created on: Nov 17, 2012
 *      Author: Greg
 */
#include "BlockTopCorners.h"

namespace gsl
{
namespace rectopia
{

struct BlockTopCorners::Impl
{
  /// Bitfield for corner heights.
  /// Since corner heights only range from -1 to +1, each height variable can
  /// be just 2 bits wide, allowing for a -2 to +1 range.  This cuts my old
  /// struct of 16 bytes down to a single byte!
  struct _corners
  {
    int back_left_ : 2;
    int back_right_ : 2;
    int front_left_ : 2;
    int front_right_ : 2;
  } corners;
};

BlockTopCorners::BlockTopCorners()
  : impl(new Impl())
{
  Reset();
}

BlockTopCorners::BlockTopCorners(const BlockTopCorners& original)
  : impl(new Impl())
{
  impl->corners = original.impl->corners;
}

BlockTopCorners::~BlockTopCorners()
{
}

void BlockTopCorners::Reset()
{
  impl->corners.back_left_ = 0;
  impl->corners.back_right_ = 0;
  impl->corners.front_left_ = 0;
  impl->corners.front_right_ = 0;
}

int BlockTopCorners::back_left()
{
  return impl->corners.back_left_;
}

int BlockTopCorners::back_right()
{
  return impl->corners.back_right_;
}

int BlockTopCorners::front_left()
{
  return impl->corners.front_left_;
}

int BlockTopCorners::front_right()
{
  return impl->corners.front_right_;
}

int BlockTopCorners::count_low_corners()
{
  return ((impl->corners.back_left_ < 0) ? 1 : 0)
         + ((impl->corners.back_right_ < 0) ? 1 : 0)
         + ((impl->corners.front_left_ < 0) ? 1 : 0)
         + ((impl->corners.front_right_ < 0) ? 1 : 0);
}

void BlockTopCorners::set_back_left(int n)
{
  impl->corners.back_left_ = n;
}

void BlockTopCorners::set_back_right(int n)
{
  impl->corners.back_right_ = n;
}

void BlockTopCorners::set_front_left(int n)
{
  impl->corners.front_left_ = n;
}

void BlockTopCorners::set_front_right(int n)
{
  impl->corners.front_right_ = n;
}

} // namespace rectopia
} // namespace gsl
