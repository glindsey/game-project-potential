#ifndef BGRENDERDATA_H
#define BGRENDERDATA_H

#include <boost/container/vector.hpp>
#include <glm/glm.hpp>

#include "common.h"

// Forward declarations
struct BGVertexRenderData;

/** Struct representing all of the rendering data associated with the background. */
struct BGRenderData
{
  BGRenderData();
  ~BGRenderData();
  void clear_vertices();
  void add_vertex(glm::vec2 vertex,
                 glm::vec4 color);
  void update_VAO();

  /// Vertex vector.
  boost::container::vector<BGVertexRenderData> vertices;

  /// VBO ID for the vertex array.
  unsigned int vbo_id;

  /// VAO ID for the vertex array.
  unsigned int vao_id;

  /// Number of vertices.
  int vertex_count;
};

#endif // CHUNKRENDERDATA_H
