#ifndef CURSORVERTEXRENDERDATA_H
#define CURSORVERTEXRENDERDATA_H

#include <glm/glm.hpp>

namespace gsl
{
namespace rectopia
{

/** Struct representing a vertex to send to openGL. */
struct CursorVertexRenderData
{
  CursorVertexRenderData()
  {
    x = 0;
    y = 0;
    z = 0;
    r = 0;
    g = 0;
    b = 0;
    a = 0;
  }

  CursorVertexRenderData(glm::vec3 _coord,
                        glm::vec4 _color)
  {
    x = _coord.x;
    y = _coord.y;
    z = _coord.z;
    r = _color.r;
    g = _color.g;
    b = _color.b;
    a = _color.a;
  }

  float x, y, z;            ///< Worldspace coordinates
  float r, g, b, a;         ///< Color
};

} // end namespace rectopia
} // end namespace gsl

#endif // CURSORVERTEXRENDERDATA_H
