#ifndef CURSORRENDERDATA_H
#define CURSORRENDERDATA_H

#include <boost/container/vector.hpp>
#include <glm/glm.hpp>

#include "common.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
struct CursorVertexRenderData;

/** Struct representing all of the rendering data associated with the cursor. */
struct CursorRenderData
{
  CursorRenderData();
  ~CursorRenderData();
  void clear_vertices();
  void add_vertex(glm::vec3 vertex,
                 glm::vec4 color);
  void update_VAOs();

  /// Vertex vector.
  boost::container::vector<CursorVertexRenderData> vertices;

  /// VBO ID for the vertex array.
  unsigned int vbo_id;

  /// VAO ID for the vertex array.
  unsigned int vao_id;

  /// Number of vertices.
  int vertex_count;
};

} // end namespace rectopia
} // end namespace gsl

#endif // CURSORRENDERDATA_H
