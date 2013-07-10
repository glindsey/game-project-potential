#ifndef VERTEXRENDERDATA_H
#define VERTEXRENDERDATA_H

#include <glm/glm.hpp>

namespace gsl
{
namespace rectopia
{

/** Struct representing a vertex to send to openGL. */
struct ChunkVertexRenderData
{
  ChunkVertexRenderData()
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
    u = 0;
    v = 0;
  }

  ChunkVertexRenderData(glm::vec3 _block_coords,
                   float _x,
                   float _y,
                   float _z,
                   float _nx,
                   float _ny,
                   float _nz,
                   float _r = 0.5,
                   float _g = 0.5,
                   float _b = 0.5,
                   float _a = 1.0,
                   float _rs = 0.5,
                   float _gs = 0.5,
                   float _bs = 0.5,
                   float _as = 1.0,
                   float _u = 0,
                   float _v = 0)
  {
    bx = _block_coords.x;
    by = _block_coords.y;
    bz = _block_coords.z;
    x = _x;
    y = _y;
    z = _z;
    nx = _nx;
    ny = _ny;
    nz = _nz;
    r = _r;
    g = _g;
    b = _b;
    a = _a;
    rs = _rs;
    gs = _gs;
    bs = _bs;
    as = _as;
    u = _u;
    v = _v;
  }

  float bx, by, bz;         ///< Block coordinates
  float x, y, z;            ///< Worldspace coordinates
  float nx, ny, nz;         ///< Normal direction
  float r, g, b, a;         ///< Color
  float rs, gs, bs, as;     ///< Specular color
  float u, v;               ///< Texture coordinates
};

} // end namespace rectopia
} // end namespace gsl

#endif // VERTEXRENDERDATA_H
