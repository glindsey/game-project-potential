#ifndef GUIRENDERDATA_H
#define GUIRENDERDATA_H

#include <boost/container/vector.hpp>
#include <glm/glm.hpp>

#include "common.h"

// Forward declarations
struct GUIVertexRenderData;

/** Struct representing all of the rendering data associated with a GUI element. */
struct GUIRenderData
{
  GUIRenderData();
  ~GUIRenderData();
  void clear_vertices();
  void add_vertex(glm::vec2 vertex,
                 glm::vec4 color,
                 float tex_coord_x = 0.0f,
                 float tex_coord_y = 0.0f,
                 bool textured = false);
  void update_VAO();

  /// Vertex vector.
  boost::container::vector<GUIVertexRenderData> vertices;

  /// VBO ID for the vertex array.
  unsigned int vbo_id;

  /// VAO ID for the vertex array.
  unsigned int vao_id;

  /// Number of vertices.
  int vertex_count;
};
#endif // CHUNKRENDERDATA_H
