#include "BGRenderData.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "BGVertexRenderData.h"

namespace gsl
{
namespace rectopia
{

BGRenderData::BGRenderData()
{
  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_id);
  clear_vertices();
}

BGRenderData::~BGRenderData()
{
  glDeleteVertexArrays(1, &vao_id);
  glDeleteBuffers(1, &vbo_id);
}

void BGRenderData::clear_vertices()
{
  vertices.clear();
  vertex_count = 0;
}

void BGRenderData::add_vertex(glm::vec2 vertex,
                              glm::vec4 color)
{
  BGVertexRenderData newVertex(vertex.x, vertex.y,
                                color.r, color.g, color.b, color.a);

  vertices.push_back(newVertex);
  ++vertex_count;
}

void BGRenderData::update_VAO()
{
  // Bind the solid VAO.
  glBindVertexArray(vao_id);

  // Bind the solid VBO.
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

  // Copy the data to the solid VBO.
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BGVertexRenderData),
               &(vertices[0]), GL_STATIC_DRAW);

  // Set up attribute 0 to be the vertex's position.
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                        sizeof(BGVertexRenderData),
                        (const void*) offsetof(BGVertexRenderData, x));

  // Set up attribute 1 to be the vertex's color.
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                        sizeof(BGVertexRenderData),
                        (const void*) offsetof(BGVertexRenderData, r));

  // Clear the vertex data, but keep the count.
  vertices.clear();

  // Unbind the VAO (for now).
  glBindVertexArray(0);
}

} // end namespace rectopia
} // end namespace gsl
