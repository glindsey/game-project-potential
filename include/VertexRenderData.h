#ifndef VERTEXRENDERDATA_H
#define VERTEXRENDERDATA_H

#include <glm/glm.hpp>

namespace gsl
{
namespace rectopia
{

/** Struct representing a vertex to send to openGL. */
struct VertexRenderData
{
  VertexRenderData()
  {
    bx = 0;
    by = 0;
    bz = 0;
    x = 0;
    y = 0;
    z = 0;
    nx = 0;
    ny = 0;
    nz = 0;
    r = 0;
    g = 0;
    b = 0;
    a = 0;
    rs = 0;
    gs = 0;
    bs = 0;
    as = 0;
    s = 0;
    t = 0;
  }

  VertexRenderData(glm::vec3 _block_coords,
                   glm::vec3 _coord,
                   glm::vec3 _normal,
                   glm::vec4 _color,
                   glm::vec4 _color_specular,
                   glm::vec2 _tex_coord)
  {
    bx = _block_coords.x;
    by = _block_coords.y;
    bz = _block_coords.z;
    x = _coord.x;
    y = _coord.y;
    z = _coord.z;
    nx = _normal.x;
    ny = _normal.y;
    nz = _normal.z;
    r = _color.r;
    g = _color.g;
    b = _color.b;
    a = _color.a;
    rs = _color_specular.r;
    gs = _color_specular.g;
    bs = _color_specular.b;
    as = _color_specular.a;
    s = _tex_coord.s;
    t = _tex_coord.t;
  }

  float bx, by, bz;         ///< Block coordinates
  float x, y, z;            ///< Worldspace coordinates
  float nx, ny, nz;         ///< Normal direction
  float r, g, b, a;         ///< Color
  float rs, gs, bs, as;     ///< Specular color
  float s, t;               ///< Texture coordinates
};

} // end namespace rectopia
} // end namespace gsl

#endif // VERTEXRENDERDATA_H
