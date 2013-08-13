#ifndef RENDERDATA_H
#define RENDERDATA_H

#include <boost/container/vector.hpp>
#include <glm/glm.hpp>

#include "common.h"

// Forward declarations
struct VertexRenderData;

/** Struct representing all of the rendering data associated with a chunk. */
struct RenderData
{
  RenderData();
  ~RenderData();
  void clear_vertices();
  void add_vertex(glm::vec3 block_coords,
                  glm::vec3 vertex,
                  glm::vec3 normal,
                  glm::vec4 color,
                  glm::vec4 color_specular,
                  glm::vec2 texCoord);

  void add_outline_vertex(glm::vec3 block_coords,
                          glm::vec3 coord,
                          glm::vec4 color,
                          glm::vec4 color_pulse);

  void update_VAOs();

  /// SOLID vertex vector.
  boost::container::vector<VertexRenderData> solid_vertices;

  /// TRANSLUCENT vertex vector.
  boost::container::vector<VertexRenderData> translucent_vertices;

  /// OUTLINE vertex vector.
  boost::container::vector<VertexRenderData> outline_vertices;

  /// VBO ID for the SOLID vertex array.
  unsigned int solid_vbo_id;

  /// VBO ID for the TRANSLUCENT vertex array.
  unsigned int translucent_vbo_id;

  /// VBO ID for the OUTLINE vertex array.
  unsigned int outline_vbo_id;

  /// VAO ID for the SOLID vertex array.
  unsigned int solid_vao_id;

  /// VAO ID for the TRANSLUCENT vertex array.
  unsigned int translucent_vao_id;

  /// VAO ID for the OUTLINE vertex array.
  unsigned int outline_vao_id;

  /// Number of SOLID vertices.
  int solid_vertex_count;

  /// Number of TRANSLUCENT vertices.
  int translucent_vertex_count;

  /// Number of OUTLINE vertices.
  int outline_vertex_count;
};
#endif // CHUNKRENDERDATA_H
