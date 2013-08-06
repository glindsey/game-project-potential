#include "ChunkRenderData.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ChunkVertexRenderData.h"

namespace gsl
{
namespace rectopia
{

ChunkRenderData::ChunkRenderData()
{
  glGenVertexArrays(1, &solid_vao_id);
  glGenVertexArrays(1, &translucent_vao_id);
  glGenBuffers(1, &solid_vbo_id);
  glGenBuffers(1, &translucent_vbo_id);
  clear_vertices();
}

ChunkRenderData::~ChunkRenderData()
{
  glDeleteVertexArrays(1, &solid_vao_id);
  glDeleteVertexArrays(1, &translucent_vao_id);
  glDeleteBuffers(1, &solid_vbo_id);
  glDeleteBuffers(1, &translucent_vbo_id);
}

void ChunkRenderData::clear_vertices()
{
  solid_vertices.clear();
  translucent_vertices.clear();
  solid_vertex_count = 0;
  translucent_vertex_count = 0;
}

void ChunkRenderData::add_vertex(glm::vec3 block_coords,
                           glm::vec3 coord,
                           glm::vec3 normal,
                           glm::vec4 color,
                           glm::vec4 color_specular,
                           glm::vec2 tex_coord)
{
  ChunkVertexRenderData vertex(block_coords, coord, normal,
                               color, color_specular, tex_coord);

  if (color.a == 1.0f)
  {
    solid_vertices.push_back(vertex);
    ++solid_vertex_count;
  }
  else
  {
    translucent_vertices.push_back(vertex);
    ++translucent_vertex_count;
  }
}

void ChunkRenderData::update_VAOs()
{
  // bind the solid VAO.
  glBindVertexArray(solid_vao_id);

  // bind the solid VBO.
  glBindBuffer(GL_ARRAY_BUFFER, solid_vbo_id);

  // Copy the data to the solid VBO.
  glBufferData(GL_ARRAY_BUFFER, solid_vertices.size() * sizeof(ChunkVertexRenderData),
               &(solid_vertices[0]), GL_STATIC_DRAW);

  // Set up attribute 0 to be the block the vertex belongs to.
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        sizeof(ChunkVertexRenderData),
                        (const void*) offsetof(ChunkVertexRenderData, bx));

  // Set up attribute 1 to be the vertex's position.
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(ChunkVertexRenderData),
                        (const void*) offsetof(ChunkVertexRenderData, x));

  // Set up attribute 2 to be the vertex's color.
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
                        sizeof(ChunkVertexRenderData),
                        (const void*) offsetof(ChunkVertexRenderData, r));

  // Set up attribute 3 to be the vertex's specular color.
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
                        sizeof(ChunkVertexRenderData),
                        (const void*) offsetof(ChunkVertexRenderData, rs));

  // Set up attribute 4 to be the vertex's normal.
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
                        sizeof(ChunkVertexRenderData),
                        (const void*) offsetof(ChunkVertexRenderData, nx));

  // Clear the vertex data, but keep the count.
  solid_vertices.clear();

  // bind the translucent VAO.
  glBindVertexArray(translucent_vao_id);

  // bind the translucent VBO.
  glBindBuffer(GL_ARRAY_BUFFER, translucent_vbo_id);

  // Upload the data to the VBO.
  glBufferData(GL_ARRAY_BUFFER, translucent_vertices.size() * sizeof(ChunkVertexRenderData),
               &(translucent_vertices[0]), GL_STATIC_DRAW);

    // Set up attribute 0 to be the block the vertex belongs to.
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        sizeof(ChunkVertexRenderData),
                        (const void*) offsetof(ChunkVertexRenderData, bx));

  // Set up attribute 1 to be the vertex's position.
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(ChunkVertexRenderData),
                        (const void*) offsetof(ChunkVertexRenderData, x));

  // Set up attribute 2 to be the vertex's color.
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
                        sizeof(ChunkVertexRenderData),
                        (const void*) offsetof(ChunkVertexRenderData, r));

  // Set up attribute 3 to be the vertex's specular color.
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
                        sizeof(ChunkVertexRenderData),
                        (const void*) offsetof(ChunkVertexRenderData, rs));

  // Set up attribute 4 to be the vertex's normal.
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
                        sizeof(ChunkVertexRenderData),
                        (const void*) offsetof(ChunkVertexRenderData, nx));

  // Clear the vertex data, but keep the count.
  translucent_vertices.clear();

  // unbind the VAO (for now).
  glBindVertexArray(0);
}

} // end namespace rectopia
} // end namespace gsl

