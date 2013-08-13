#ifndef CUBICBEZIER_H
#define CUBICBEZIER_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include <SFML/Graphics.hpp>

/// Representation of a 2-D cubic Bezier curve.
class CubicBezier
{
public:
  CubicBezier(sf::Vector2f const& _a,
              sf::Vector2f const& _b,
              sf::Vector2f const& _c,
              sf::Vector2f const& _d);
  ~CubicBezier();

  sf::Vector2f get_value(float t);

protected:
private:
  struct Impl;

  /// Private Implementation Pointer
  std::unique_ptr<Impl> impl;
};

#endif // CUBICBEZIER_H
