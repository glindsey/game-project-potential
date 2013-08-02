#include "GUIRenderData.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GUIVertexRenderData.h"

namespace gsl
{
namespace rectopia
{

GUIRenderData::GUIRenderData()
{
  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_id);
  clear_vertices();
}

GUIRenderData::~GUIRenderData()
{
  glDeleteVertexArrays(1, &vao_id);
  glDeleteBuffers(1, &vbo_id);
}

void GUIRenderData::clear_vertices()
{
  vertices.clear();
  vertex_count = 0;
}

void GUIRenderData::add_vertex(glm::vec2 vertex,
                           glm::vec4 color,
                           float tex_coord_x,
                           float tex_coord_y,
                           bool textured)
{
  GUIVertexRenderData newVertex(vertex.x, vertex.y,
                             color.r, color.g, color.b, color.a,
                             tex_coord_x, tex_coord_y, (textured ? 1.0 : 0.0));

  vertices.push_back(newVertex);
  ++vertex_count;
}

void GUIRenderData::update_VAO()
{
  // Bind the solid VAO.
  glBindVertexArray(vao_id);

  // Bind the solid VBO.
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

  // Copy the data to the solid VBO.
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GUIVertexRenderData),
               &(vertices[0]), GL_STATIC_DRAW);

  // Set up attribute 0 to be the vertex's position.
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                        sizeof(GUIVertexRenderData),
                        (const void*) offsetof(GUIVertexRenderData, x));

  // Set up attribute 1 to be the vertex's color.
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                        sizeof(GUIVertexRenderData),
                        (const void*) offsetof(GUIVertexRenderData, r));

  // Set up attribute 2 to be the vertex's texture coordinates.
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                        sizeof(GUIVertexRenderData),
                        (const void*) offsetof(GUIVertexRenderData, s));

  // Set up attribute 3 to be whether the vertex is textured.
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
                        sizeof(GUIVertexRenderData),
                        (const void*) offsetof(GUIVertexRenderData, textured));

  // Clear the vertex data, but keep the count.
  vertices.clear();

  // Unbind the VAO (for now).
  glBindVertexArray(0);
}

} // end namespace rectopia
} // end namespace gsl
