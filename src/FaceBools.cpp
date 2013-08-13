/*
 * FaceBools.cpp
 *
 *  Created on: Nov 9, 2012
 *      Author: Gregory Lindsey
 */

#include "FaceBools.h"

struct FaceBools::Impl
{
  struct _faces
  {
    bool top : 1;
    bool right : 1;
    bool front : 1;
    bool bottom : 1;
    bool left : 1;
    bool back : 1;
    int : 2;      /// unused bits
  } faces;
};

FaceBools::FaceBools()
  : impl(new Impl())
{
  setAll(false);
}

FaceBools::FaceBools(bool b)
  : impl(new Impl())
{
  setAll(b);
}

FaceBools::~FaceBools()
{
}

FaceBools::FaceBools(const FaceBools& original)
  : impl(new Impl())
{
  impl->faces = original.impl->faces;
}

void FaceBools::setAll(bool b)
{
  impl->faces.top = b;
  impl->faces.right = b;
  impl->faces.front = b;
  impl->faces.bottom = b;
  impl->faces.left = b;
  impl->faces.back = b;
}

bool FaceBools::anyTrue() const
{
  return (impl->faces.top || impl->faces.right || impl->faces.front
          || impl->faces.bottom || impl->faces.left || impl->faces.back);
}

bool FaceBools::allTrue() const
{
  return (impl->faces.top && impl->faces.right && impl->faces.front
          && impl->faces.bottom && impl->faces.left && impl->faces.back);
}

bool FaceBools::top() const
{
  return impl->faces.top;
}
bool FaceBools::bottom() const
{
  return impl->faces.bottom;
}
bool FaceBools::left() const
{
  return impl->faces.left;
}
bool FaceBools::right() const
{
  return impl->faces.right;
}
bool FaceBools::back() const
{
  return impl->faces.back;
}
bool FaceBools::front() const
{
  return impl->faces.front;
}

void FaceBools::set_top(bool value)
{
  impl->faces.top = value;
}
void FaceBools::set_bottom(bool value)
{
  impl->faces.bottom = value;
}
void FaceBools::set_left(bool value)
{
  impl->faces.left = value;
}
void FaceBools::set_right(bool value)
{
  impl->faces.right = value;
}
void FaceBools::set_back(bool value)
{
  impl->faces.back = value;
}
void FaceBools::set_front(bool value)
{
  impl->faces.front = value;
}

FaceBools& FaceBools::operator=(const FaceBools& rhs)
{
  if (this == &rhs)
  {
    return *this;
  }

  impl->faces.back = rhs.impl->faces.back;
  impl->faces.front = rhs.impl->faces.front;
  impl->faces.left = rhs.impl->faces.left;
  impl->faces.right = rhs.impl->faces.right;
  impl->faces.bottom = rhs.impl->faces.bottom;
  impl->faces.top = rhs.impl->faces.top;

  return *this;
}

FaceBools& FaceBools::operator=(const bool& rhs)
{
  impl->faces.back = rhs;
  impl->faces.front = rhs;
  impl->faces.left = rhs;
  impl->faces.right = rhs;
  impl->faces.bottom = rhs;
  impl->faces.top = rhs;

  return *this;
}

FaceBools& FaceBools::operator|=(const FaceBools& rhs)
{
  impl->faces.back |= rhs.impl->faces.back;
  impl->faces.front |= rhs.impl->faces.front;
  impl->faces.left |= rhs.impl->faces.left;
  impl->faces.right |= rhs.impl->faces.right;
  impl->faces.bottom |= rhs.impl->faces.bottom;
  impl->faces.top |= rhs.impl->faces.top;

  return *this;
}

FaceBools& FaceBools::operator&=(const FaceBools& rhs)
{
  impl->faces.back &= rhs.impl->faces.back;
  impl->faces.front &= rhs.impl->faces.front;
  impl->faces.left &= rhs.impl->faces.left;
  impl->faces.right &= rhs.impl->faces.right;
  impl->faces.bottom &= rhs.impl->faces.bottom;
  impl->faces.top &= rhs.impl->faces.top;

  return *this;
}

FaceBools& FaceBools::operator^=(const FaceBools& rhs)
{
  impl->faces.back ^= rhs.impl->faces.back;
  impl->faces.front ^= rhs.impl->faces.front;
  impl->faces.left ^= rhs.impl->faces.left;
  impl->faces.right ^= rhs.impl->faces.right;
  impl->faces.bottom ^= rhs.impl->faces.bottom;
  impl->faces.top ^= rhs.impl->faces.top;

  return *this;
}

const FaceBools FaceBools::operator|(const FaceBools& other) const
{
  FaceBools result = *this;
  result |= other;
  return result;
}

const FaceBools FaceBools::operator&(const FaceBools& other) const
{
  FaceBools result = *this;
  result &= other;
  return result;
}

const FaceBools FaceBools::operator^(const FaceBools& other) const
{
  FaceBools result = *this;
  result ^= other;
  return result;
}

bool FaceBools::operator==(const FaceBools& other) const
{
  return ((impl->faces.back == other.impl->faces.back)
          && (impl->faces.front == other.impl->faces.front)
          && (impl->faces.left == other.impl->faces.left)
          && (impl->faces.right == other.impl->faces.right)
          && (impl->faces.bottom == other.impl->faces.bottom)
          && (impl->faces.top == other.impl->faces.top));
}

bool FaceBools::operator!=(const FaceBools& other) const
{
  return !(*this == other);
}

FaceBools FaceBools::operator~()
{
  FaceBools result;
  result.impl->faces.back = !impl->faces.back;
  result.impl->faces.front = !impl->faces.front;
  result.impl->faces.left = !impl->faces.left;
  result.impl->faces.right = !impl->faces.right;
  result.impl->faces.bottom = !impl->faces.bottom;
  result.impl->faces.top = !impl->faces.top;
  return result;
}

FaceBools FaceBools::operator!()
{
  return this->operator~();
}
