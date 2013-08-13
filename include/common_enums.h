#ifndef COMMON_ENUMS_H_INCLUDED
#define COMMON_ENUMS_H_INCLUDED

/// Enumeration of layers comprising a block.
enum class BlockLayer
{
  Solid, Fluid, Cover, Count
};

enum class Octant
{
  BottomBackLeft,
  BottomBackRight,
  BottomFrontLeft,
  BottomFrontRight,
  TopBackLeft,
  TopBackRight,
  TopFrontLeft,
  TopFrontRight,
  Count
};

enum class DepositType
{
  None,
  Stratum,
  LargeDeposit,
  SmallDeposit,
  Vein,
  Single,
  Gangue,
  Liquid,
  Unknown,
  Count
};

enum class FaceName
{
  Top = 0,
  Right = 1,
  Front = 2,
  Bottom = 3,
  Left = 4,
  Back = 5
};

enum class Heading
{
  North,
  Northeast,
  East,
  Southeast,
  South,
  Southwest,
  West,
  Northwest,
  MemberCount
};

enum class OrientationStyle
{
  Static, FourPoint, EightPoint, MemberCount
};

enum class Phase
{
  None, Solid, Viscous, Liquid, Gas, Ethereal, Unknown, MemberCount
};

enum class ViewType
{
  Oblique, Flat, Tiny, MemberCount
};

/// Indicates visibility of a material.
/// "Translucent" does not necessarily mean translucent in the traditional
/// sense; it merely indicates that the material has non-opaque portions
/// through which underlying graphics can be seen.
enum class Visibility
{
  Invisible, Translucent, Opaque
};

/// Definition of PI.
const double PI = 3.141592653589791L;

#endif // COMMON_ENUMS_H_INCLUDED
