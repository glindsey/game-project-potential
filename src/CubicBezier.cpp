#include "CubicBezier.h"

namespace gsl
{
namespace rectopia
{

struct CubicBezier::Impl
{
  /// Simple linear interpolation between two points.
  sf::Vector2f lerp(sf::Vector2f const& a,
                    sf::Vector2f const& b,
                    float const t);

  /// These points define the start, attractors, and end of the curve.
  sf::Vector2f a, b, c, d;

  /// These values are used for intermediate computation.
  sf::Vector2f ab, bc, cd, abbc, bccd, dest;
};

CubicBezier::CubicBezier(const sf::Vector2f& _a,
                         const sf::Vector2f& _b,
                         const sf::Vector2f& _c,
                         const sf::Vector2f& _d)
  : impl(new Impl())
{
  impl->a = _a;
  impl->b = _b;
  impl->c = _c;
  impl->d = _d;
}

CubicBezier::~CubicBezier()
{

}

sf::Vector2f CubicBezier::get_value(float t)
{
  impl->ab = impl->lerp(impl->a, impl->b, t);   // point between a and b (green)
  impl->bc = impl->lerp(impl->b, impl->c, t);   // point between b and c (green)
  impl->cd = impl->lerp(impl->c, impl->d, t);   // point between c and d (green)
  impl->abbc = impl->lerp(impl->ab, impl->bc, t); // point between ab and bc (blue)
  impl->bccd = impl->lerp(impl->bc, impl->cd, t); // point between bc and cd (blue)
  impl->dest = impl->lerp(impl->abbc, impl->bccd, t); // point on the bezier-curve (black)

  return impl->dest;
}

sf::Vector2f CubicBezier::Impl::lerp(const sf::Vector2f& a,
                                     const sf::Vector2f& b,
                                     const float t)
{
  sf::Vector2f dest;

  dest.x = a.x + (b.x - a.x) * t;
  dest.y = a.y + (b.y - a.y) * t;

  return dest;
}

} // namespace rectopia
} // namespace gsl
