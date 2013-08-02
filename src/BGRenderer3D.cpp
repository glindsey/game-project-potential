#include "BGRenderer3D.h"

#include <algorithm>
#include <iterator>
#include <vector>

#include "Application.h"
#include "BGRenderData.h"
#include "GLShaderProgram.h"

namespace gsl
{
namespace rectopia
{

struct BGRenderer3D::Impl
{
  void draw_gradient_rect(float left,
                          float right,
                          float top,
                          float bottom,
                          glm::vec4 top_left_color,
                          glm::vec4 top_right_color,
                          glm::vec4 bottom_left_color,
                          glm::vec4 bottom_right_color)
  {
    render_data_->add_vertex(glm::vec2(left, top), top_left_color);
    render_data_->add_vertex(glm::vec2(right, top), top_right_color);
    render_data_->add_vertex(glm::vec2(left, bottom), bottom_left_color);
    render_data_->add_vertex(glm::vec2(left, bottom), bottom_left_color);
    render_data_->add_vertex(glm::vec2(right, bottom), bottom_right_color);
    render_data_->add_vertex(glm::vec2(right, top), top_right_color);
  }

  std::unique_ptr<BGRenderData> render_data_;
  std::unique_ptr<GLShaderProgram> shader_program_; ///< Rendering program

  // IDs for uniforms.
  GLuint window_size_id_;
  GLuint frame_counter_id_;

  bool vao_dirty_;  ///< Boolean indicating whether VAO needs updating
};

BGRenderer3D::BGRenderer3D()
  : impl(new Impl())
{
  name_ = "3D Renderer";
  impl->render_data_.reset(new BGRenderData());
  // Create and compile our GLSL program from the shaders
  impl->shader_program_.reset(
    new GLShaderProgram("shaders/BGVertexShader.glsl",
                        "shaders/BGFragmentShader.glsl"));

  // Get IDs for uniforms.
  impl->frame_counter_id_ = impl->shader_program_->getUniformId("frame_counter");

  impl->render_data_->clear_vertices();

  printf("Drawing skybox\n");
  impl->draw_gradient_rect(-1.0f, 1.0f,
                           -1.0f, 1.0f,
                           glm::vec4(0.00, 0.75, 1.00, 1.00),
                           glm::vec4(0.00, 0.75, 1.00, 1.00),
                           glm::vec4(0.00, 0.00, 0.25, 1.00),
                           glm::vec4(0.00, 0.00, 0.25, 1.00));

  impl->vao_dirty_ = true;
}

BGRenderer3D::~BGRenderer3D()
{

}

void BGRenderer3D::prepare()
{
  // Use our shader program.
  impl->shader_program_->Bind();

  // Clear everything in preparation for a new frame.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void BGRenderer3D::draw()
{
  static unsigned int frame_counter;

  if (impl->vao_dirty_)
  {
    impl->render_data_->update_VAO();
    impl->vao_dirty_ = false;
  }

  // Send frame counter to renderer.
  glUniform1f(impl->frame_counter_id_, frame_counter);

  glBindVertexArray(impl->render_data_->vao_id);
  glDrawArrays(GL_TRIANGLES, 0, impl->render_data_->vertex_count);

  ++frame_counter;
}

void BGRenderer3D::finish()
{
  glBindVertexArray(0);

  // Use our shader program.
  impl->shader_program_->Unbind();
}



} // end namespace rectopia
} // end namespace gsl
