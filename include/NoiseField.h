#ifndef NOISEFIELD_H
#define NOISEFIELD_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

namespace gsl
{
namespace rectopia
{

/// A two-dimensional height map used to generate terrain, forests,
/// anything that needs smooth noise.
/// @todo Dump this.  libnoise works far better.
class NoiseField
{
public:
  NoiseField(int _xySize, int _zLo, int _zHi, int _variance, int _seed);
  ~NoiseField();

  int getScaledValue(int x, int y);

protected:
private:
  struct Impl;
  /// Private implementation pointer
  boost::scoped_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif // HEIGHTMAP_H
