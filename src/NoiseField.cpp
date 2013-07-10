// *** ADDED BY HEADER FIXUP ***
#include <algorithm>
#include <iostream>
#include <vector>
// *** END ***
#include "NoiseField.h"
#include <ctime>

namespace gsl
{
namespace rectopia
{

struct NoiseField::Impl
{
  void initialize(int _xySize, int _zLo, int _zHi, int _variance, int _seed);
  void generate();
  int coord(int x, int y);
  int getWrappedValue(int x, int y);

  int xySize;
  int zLo, zHi;
  int variance;

  /// Random number generator used for terrain generation.
  boost::random::mt19937 twister;

  /// Seed used for seeding twister.
  int seed;

  /// Vector of height values.
  boost::container::vector<int> values;
};

void NoiseField::Impl::initialize(int _xySize,
                                  int _zLo,
                                  int _zHi,
                                  int _variance,
                                  int _seed)
{
  std::cout << "DEBUG: init Noisefield: Size = " << _xySize << ", Z = (" << _zLo
            << " to " << _zHi << "), Variance = " << _variance << std::endl;
  xySize = _xySize;
  zLo = _zLo;
  zHi = _zHi;
  variance = _variance;
  seed = _seed;
  twister.seed(seed);

  generate();
}

/// We generate a noise field that is 1024x1024x16385 in size.
void NoiseField::Impl::generate()
{
  int sideLength = xySize;
  int xCoord = 0, yCoord = 0;
  int zAvg = 0, zNew = 0;
  int zMax = 16384;
  int offset = variance;

  // Set up our vector.
  values.resize(xySize * xySize);

  // Start the corner value off.
  values[coord(0, 0)] = 8192;

  do
  {
    int halfLength = (sideLength >> 1);

    std::cout << "DEBUG: Side length = " << sideLength << std::endl;

    boost::random::uniform_int_distribution<> dist(-offset, offset);

    // Diamond step.
    for (yCoord = 0; yCoord < xySize; yCoord += sideLength)
    {
      for (xCoord = 0; xCoord < xySize; xCoord += sideLength)
      {
        // Calculate average of square @ (xc + h, yc + h).
        zAvg = (getWrappedValue(xCoord, yCoord)
                + getWrappedValue(xCoord, yCoord + sideLength)
                + getWrappedValue(xCoord + sideLength, yCoord)
                + getWrappedValue(xCoord + sideLength, yCoord + sideLength))
               >> 2;

        zNew = zAvg + dist(twister);
        if (zNew < 0)
        {
          zNew = 0;
        }
        if (zNew > zMax)
        {
          zNew = zMax;
        }

        values[coord(xCoord + halfLength, yCoord + halfLength)] = zNew;
      }
    }

    // Square step.
    for (yCoord = 0; yCoord < xySize; yCoord += sideLength)
    {
      for (xCoord = 0; xCoord < xySize; xCoord += sideLength)
      {
        // Calculate top diamond point @ (xc + h, yc).
        zAvg = (getWrappedValue(xCoord + halfLength, yCoord - halfLength)
                + getWrappedValue(xCoord, yCoord)
                + getWrappedValue(xCoord + sideLength, yCoord)
                + getWrappedValue(xCoord + halfLength, yCoord + halfLength))
               >> 2;

        zNew = zAvg + dist(twister);
        if (zNew < 0)
        {
          zNew = 0;
        }
        if (zNew > zMax)
        {
          zNew = zMax;
        }

        values[coord(xCoord + halfLength, yCoord)] = zNew;

        // Calculate left diamond point @ (xc, yc + h).
        zAvg = (getWrappedValue(xCoord, yCoord)
                + getWrappedValue(xCoord - halfLength, yCoord + halfLength)
                + getWrappedValue(xCoord + halfLength, yCoord + halfLength)
                + getWrappedValue(xCoord, yCoord + sideLength))
               >> 2;

        zNew = zAvg + dist(twister);
        if (zNew < 0)
        {
          zNew = 0;
        }
        if (zNew > zMax)
        {
          zNew = zMax;
        }

        values[coord(xCoord, yCoord + halfLength)] = zNew;
      }
    }

    // TODO: another magic number!
    offset >>= 2;
    sideLength = halfLength;
  }
  while (sideLength > 1);
}

int NoiseField::Impl::coord(int x, int y)
{
  return (y * xySize) + x;
}

int NoiseField::Impl::getWrappedValue(int x, int y)
{
  while (x < 0)
  {
    x += xySize;
  }
  while (y < 0)
  {
    y += xySize;
  }
  x = x % xySize;
  y = y % xySize;
  return values[coord(x, y)];
}

NoiseField::NoiseField(int _xySize,
                       int _zLo,
                       int _zHi,
                       int _variance = 16384,
                       int _seed = 0)
  : impl(new Impl())
{
  if (_seed == 0)
  {
    _seed = (int) std::time(nullptr);
  }

  impl->initialize(_xySize, _zLo, _zHi, _variance, _seed);
}

NoiseField::~NoiseField()
{
}

// TODO: Scale based on x, y size as well?
int NoiseField::getScaledValue(int x, int y)
{
  int value = impl->getWrappedValue(x, y);
  int zRange = (impl->zHi - impl->zLo) + 1;
  int scaledValue = ((value * zRange) / 16384) + (float) impl->zLo;

  return (int) scaledValue;
}

} // namespace rectopia
} // namespace gsl
