#ifndef BLOCKTOPCORNERS_H_
#define BLOCKTOPCORNERS_H_

#include "common_includes.h"
#include "common_typedefs.h"

namespace gsl
{
namespace rectopia
{

class BlockTopCorners
{
public:
  BlockTopCorners();
  BlockTopCorners(const BlockTopCorners& original);
  virtual ~BlockTopCorners();

  void Reset();

  int back_left();
  int back_right();
  int front_left();
  int front_right();
  int count_low_corners();

  void set_back_left(int n);
  void set_back_right(int n);
  void set_front_left(int n);
  void set_front_right(int n);

private:
  struct Impl;
  /// Private implementation pointer
  boost::scoped_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif /* BLOCKTOPCORNERS_H_ */
