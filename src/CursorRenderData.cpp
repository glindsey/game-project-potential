#include "CursorRenderData.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "CursorVertexRenderData.h"

namespace gsl
{
namespace rectopia
{

CursorRenderData::CursorRenderData()
{
  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_id);
  clearVertices();
}

CursorRenderData::~CursorRenderData()
{
  glDeleteVertexArrays(1, &vao_id);
  glDeleteBuffers(1, &vbo_id);
}

void CursorRenderData::clearVertices()
{
  vertices.clear();
  vertex_count = 0;
}

void CursorRenderData::addVertex(glm::vec3 coord, glm::vec4 color)
{
  CursorVertexRenderData vertex(coord, color);

  vertices.push_back(vertex);
  ++vertex_count;
}

void CursorRenderData::updateVAOs()
{
  // Bind the VAO.
  glBindVertexArray(vao_id);

  // Bind the VBO.
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

  // Copy the data to the VBO.
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CursorVertexRenderData),
               &(vertices[0]), GL_STATIC_DRAW);

  // Set up attribute 0 to be the vertex's position.
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        sizeof(CursorVertexRenderData),
                        (const void*) offsetof(CursorVertexRenderData, x));

  // Set up attribute 1 to be the vertex's color.
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                        sizeof(CursorVertexRenderData),
                        (const void*) offsetof(CursorVertexRenderData, r));

  // Clear the vertex data, but keep the count.
  vertices.clear();

  // Unbind the VAO (for now).
  glBindVertexArray(0);
}

} // end namespace rectopia
} // end namespace gsl

