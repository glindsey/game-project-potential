#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <complex>
#include <iterator>
#include <glm/glm.hpp>

#define PI 3.141592653589791

/// Converts degrees to radians.
inline float deg_to_rad(float degrees)
{
  return (degrees * (PI/180));
}

/// Convert radians to degrees.
inline float rad_to_deg(float radians)
{
  return (radians * (180/PI));
}

/// Converts polar coordinates to rectangular ones.
/// This function translates polar coordinates, centered at (0, 0), to
/// rectangular ones.
/// @param r Radius of the circle.
/// @param theta Angle, from 0 to 2*PI.
inline glm::vec2 PolarToRectangular(float r, float theta)
{
  glm::vec2 coord;
  coord.x = r * cos(theta);
  coord.y = r * sin(theta);
  return coord;
}

/// Converts spherical coordinates to cubical ones.
/// This function translates spherical coordinates, centered at (0, 0, 0),
/// to cubical ones.  To describe a sphere, rotate theta from 0 to 2*PI, and
/// phi from 0 to PI.  To describe a ball, do the above while sweeping r from 0
/// to your desired radius.
/// @param r Radius of the sphere.
/// @param theta Polar angle, from 0 to PI.
/// @param phi Azimuth angle, from 0 to 2*PI.
inline glm::vec3 SphericalToCubical(float r, float theta, float phi)
{
  glm::vec3 coord;
  coord.x = r * cos(theta) * sin(phi);
  coord.y = r * sin(theta) * sin(phi);
  coord.z = r * cos(phi);
  return coord;
}

/// Constrains a value to fall between lo_bound and hi_bound.
/// @param lo_bound Lower boundary.
/// @param variable Variable to constrain.
/// @param hi_bound Upper boundary.
/// @return Variable value constrained between lo_bound and hi_bound.
template<typename T> inline T forceBounds(T lo_bound, T variable, T hi_bound)
{
  return (
           (variable < lo_bound) ? lo_bound :
           ((variable > hi_bound) ? hi_bound : variable));
}

/// Constrains a 3-D coordinate to fall between lo_bound and hi_bound.
/// @param lo_bound Lower boundary.
/// @param variable Variable to constrain.
/// @param hi_bound Upper boundary.
/// @return Variable value constrained between lo_bound and hi_bound.
inline StageCoord3 constrain_to_box(StageCoord3 lo_bound,
                                   StageCoord3 variable,
                                   StageCoord3 hi_bound)
{
  StageCoord3 result;
  result.x =
    (variable.x < lo_bound.x) ?
    lo_bound.x : ((variable.x > hi_bound.x) ? hi_bound.x : variable.x);
  result.y =
    (variable.y < lo_bound.y) ?
    lo_bound.y : ((variable.y > hi_bound.y) ? hi_bound.y : variable.y);
  result.z =
    (variable.z < lo_bound.z) ?
    lo_bound.z : ((variable.z > hi_bound.z) ? hi_bound.z : variable.z);

  return result;
}

/// Indicates whether a value is between lo_bound and hi_bound.
/// @param lo_bound Lower boundary.
/// @param variable Variable to check.
/// @param hi_bound Upper boundary.
/// @return True if the integer falls within the bounds,
///         false if it is outside.
template<typename T> inline bool inBounds(T lo_bound, T variable, T hi_bound)
{
  return ((variable >= lo_bound) && (variable < hi_bound));
}

/// Returns true if the two 3-D coordinates share an axis.
/// @param vec1 Coordinates of first point.
/// @param vec2 Coordinates of second point.
/// @return True if the coordinates both fall on the X, Y, or Z axis;
///         false otherwise.
inline bool onAxis(StageCoord3 vec1, StageCoord3 vec2)
{
  return (((vec1.x == vec2.x) && (vec1.y == vec2.y))
          || ((vec1.x == vec2.x) && (vec1.z == vec2.z))
          || ((vec1.y == vec2.y) && (vec1.z == vec2.z)));
}

/// Calculate the normal of a triangle.
inline glm::vec3 calculateNormal(const glm::vec3 v0,
                                 const glm::vec3 v1,
                                 const glm::vec3 v2)
{
  glm::vec3 d1, d2;
  d1.x = v1.x - v0.x;
  d1.y = v1.y - v0.y;
  d1.z = v1.z - v0.z;

  d2.x = v2.x - v0.x;
  d2.y = v2.y - v0.y;
  d2.z = v2.z - v0.z;

  glm::vec3 cross_product;
  cross_product.x = d1.y * d2.z - d1.z * d2.y;
  cross_product.y = d1.z * d2.x - d1.x * d2.z;
  cross_product.z = d1.x * d2.y - d1.y * d2.x;

  float distance = sqrt(
                     (cross_product.x * cross_product.x) + (cross_product.y * cross_product.y)
                     + (cross_product.z * cross_product.z));

  glm::vec3 normal;
  normal.x = cross_product.x / distance;
  normal.y = cross_product.y / distance;
  normal.z = cross_product.z / distance;

  return normal;
}

/// Returns the sign of a value, as -1, 0, or 1.
template<typename T> int sgn(T val)
{
  return (T(0) < val) - (val < T(0));
}

/* The following operator overrides make operations on sf::Vector* classes WAY easier to read. */

inline StageCoord3 operator+(const StageCoord3& left,
                             const StageCoord3& right)
{
  return StageCoord3(left.x + right.x, left.y + right.y, left.z + right.z);
}

inline StageCoord3 operator+(const unsigned int& left,
                             const StageCoord3& right)
{
  return StageCoord3(left + right.x, left + right.y, left + right.z);
}

inline StageCoord3 operator+(const StageCoord3& left,
                             const unsigned int& right)
{
  return StageCoord3(left.x + right, left.y + right, left.z + right);
}

inline StageCoord3 operator-(const StageCoord3 arg)
{
  return StageCoord3(-arg.x, -arg.y, -arg.z);
}

inline StageCoord3 operator-(const StageCoord3& left,
                             const StageCoord3& right)
{
  return operator+(left, -right);
}

inline StageCoord3 operator-(const unsigned int& left,
                             const StageCoord3& right)
{
  return operator+(left, -right);
}

inline StageCoord3 operator-(const StageCoord3& left,
                             const unsigned int& right)
{
  return operator+(left, -right);
}

inline StageCoord3 operator*(const StageCoord3& left,
                             const StageCoord3& right)
{
  return StageCoord3(left.x * right.x, left.y * right.y, left.z * right.z);
}

inline StageCoord3 operator*(const unsigned int& left,
                             const StageCoord3& right)
{
  return StageCoord3(left * right.x, left * right.y, left * right.z);
}

inline StageCoord3 operator*(const StageCoord3& left,
                             const unsigned int& right)
{
  return StageCoord3(left.x * right, left.y * right, left.z * right);
}

inline StageCoord3 operator/(const StageCoord3& left,
                             const StageCoord3& right)
{
  return StageCoord3(left.x / right.x, left.y / right.y, left.z / right.z);
}

inline StageCoord3 operator%(const StageCoord3& left,
                             const StageCoord3& right)
{
  return StageCoord3(left.x % right.x, left.y % right.y, left.z % right.z);
}

inline StageCoord3 operator<<(const StageCoord3& left,
                              const StageCoord3& right)
{
  return StageCoord3(left.x << right.x, left.y << right.y, left.z << right.z);
}

inline StageCoord3 operator<<(const unsigned int& left,
                              const StageCoord3& right)
{
  return StageCoord3(left << right.x, left << right.y, left << right.z);
}

inline StageCoord3 operator<<(const StageCoord3& left,
                              const unsigned int& right)
{
  return StageCoord3(left.x << right, left.y << right, left.z << right);
}

inline StageCoord3 operator>>(const StageCoord3& left,
                              const StageCoord3& right)
{
  return StageCoord3(left.x >> right.x, left.y >> right.y, left.z >> right.z);
}

inline StageCoord3 operator>>(const unsigned int& left,
                              const StageCoord3& right)
{
  return StageCoord3(left >> right.x, left >> right.y, left >> right.z);
}

inline StageCoord3 operator>>(const StageCoord3& left,
                              const unsigned int& right)
{
  return StageCoord3(left.x >> right, left.y >> right, left.z >> right);
}

inline StageCoord3 operator|(const StageCoord3& left,
                             const StageCoord3& right)
{
  return StageCoord3(left.x | right.x, left.y | right.y, left.z | right.z);
}

inline StageCoord3 operator|(const unsigned int& left,
                             const StageCoord3& right)
{
  return StageCoord3(left | right.x, left | right.y, left | right.z);
}

inline StageCoord3 operator|(const StageCoord3& left,
                             const unsigned int& right)
{
  return StageCoord3(left.x | right, left.y | right, left.z | right);
}

inline StageCoord3 operator&(const StageCoord3& left,
                             const StageCoord3& right)
{
  return StageCoord3(left.x & right.x, left.y & right.y, left.z & right.z);
}

inline StageCoord3 operator&(const unsigned int& left,
                             const StageCoord3& right)
{
  return StageCoord3(left & right.x, left & right.y, left & right.z);
}

inline StageCoord3 operator&(const StageCoord3& left,
                             const unsigned int& right)
{
  return StageCoord3(left.x & right, left.y & right, left.z & right);
}

/// Return a random number between 0 and range-1.
inline unsigned int randint(unsigned int range)
{
  return rand() % range;
}

#endif // MATHUTILS_H
