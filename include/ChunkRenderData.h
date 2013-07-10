#ifndef CHUNKRENDERDATA_H
#define CHUNKRENDERDATA_H

#include <boost/container/vector.hpp>
#include <glm/glm.hpp>

#include "common.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
struct ChunkVertexRenderData;

/** Struct representing all of the rendering data associated with a chunk. */
struct ChunkRenderData
{
  ChunkRenderData();
  ~ChunkRenderData();
  void clearVertices();
  void addVertex(glm::vec3 block_coords,
                 glm::vec3 vertex,
                 glm::vec3 normal,
                 glm::vec4 color,
                 glm::vec4 color_specular,
                 glm::vec2 texCoord);
  void updateVAOs();

  /// SOLID vertex vector.
  boost::container::vector<ChunkVertexRenderData> solid_vertices;

  /// TRANSLUCENT vertex vector.
  boost::container::vector<ChunkVertexRenderData> translucent_vertices;

  /// VBO ID for the SOLID vertex array.
  unsigned int solid_vbo_id;

  /// VBO ID for the TRANSLUCENT vertex array.
  unsigned int translucent_vbo_id;

  /// VAO ID for the SOLID vertex array.
  unsigned int solid_vao_id;

  /// VAO ID for the TRANSLUCENT vertex array.
  unsigned int translucent_vao_id;

  /// Number of SOLID vertices.
  int solid_vertex_count;

  /// Number of TRANSLUCENT vertices.
  int translucent_vertex_count;
};

} // end namespace rectopia
} // end namespace gsl

#endif // CHUNKRENDERDATA_H
