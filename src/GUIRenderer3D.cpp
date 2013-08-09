#include "GUIRenderer3D.h"

#include <algorithm>
#include <iterator>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Application.h"
#include "ErrorMacros.h"
#include "GUI.h"
#include "GUIElement.h"
#include "GUIFrame.h"
#include "GUILabel.h"
#include "GUIParentElement.h"
#include "GUIRenderData.h"
#include "GLShaderProgram.h"
#include "TextureFont.h"

namespace gsl
{
namespace rectopia
{

struct GUIRenderer3D::Impl
{
  void draw_rect(float left,
                float right,
                float top,
                float bottom,
                glm::vec4 rect_color,
                unsigned int border_width = 0,
                glm::vec4 border_color = glm::vec4(1.0f),
                glm::vec4 texture_coords = glm::vec4(0.0f),
                bool textured = false)
  {
    float texture_left = texture_coords.s;
    float texture_top = texture_coords.t;
    float texture_right = texture_coords.s + texture_coords.p;
    float texture_bottom = texture_coords.t + texture_coords.q;

    // Draw the background rect as two triangles.
    if (textured == true)
    {
      render_data->add_vertex(glm::vec2(left, top), border_color, texture_left, texture_top, textured);
      render_data->add_vertex(glm::vec2(right, top), border_color, texture_right, texture_top, textured);
      render_data->add_vertex(glm::vec2(left, bottom), border_color, texture_left, texture_bottom, textured);
      render_data->add_vertex(glm::vec2(left, bottom), border_color, texture_left, texture_bottom, textured);
      render_data->add_vertex(glm::vec2(right, bottom), border_color, texture_right, texture_bottom, textured);
      render_data->add_vertex(glm::vec2(right, top), border_color, texture_right, texture_top, textured);
    }
    else
    {
      render_data->add_vertex(glm::vec2(left, top), rect_color, texture_left, texture_top, textured);
      render_data->add_vertex(glm::vec2(right, top), rect_color, texture_right, texture_top, textured);
      render_data->add_vertex(glm::vec2(left, bottom), rect_color, texture_left, texture_bottom, textured);
      render_data->add_vertex(glm::vec2(left, bottom), rect_color, texture_left, texture_bottom, textured);
      render_data->add_vertex(glm::vec2(right, bottom), rect_color, texture_right, texture_bottom, textured);
      render_data->add_vertex(glm::vec2(right, top), rect_color, texture_right, texture_top, textured);
    }

    if (border_width > 0)
    {
      render_data->add_vertex(glm::vec2(left, top), border_color);
      render_data->add_vertex(glm::vec2(right, top), border_color);
      render_data->add_vertex(glm::vec2(right, top + border_width), border_color);
      render_data->add_vertex(glm::vec2(right, top + border_width), border_color);
      render_data->add_vertex(glm::vec2(left, top + border_width), border_color);
      render_data->add_vertex(glm::vec2(left, top), border_color);

      render_data->add_vertex(glm::vec2(left, bottom), border_color);
      render_data->add_vertex(glm::vec2(right, bottom), border_color);
      render_data->add_vertex(glm::vec2(right, bottom - border_width), border_color);
      render_data->add_vertex(glm::vec2(right, bottom - border_width), border_color);
      render_data->add_vertex(glm::vec2(left, bottom - border_width), border_color);
      render_data->add_vertex(glm::vec2(left, bottom), border_color);

      render_data->add_vertex(glm::vec2(left, top + border_width), border_color);
      render_data->add_vertex(glm::vec2(left + border_width, top + border_width), border_color);
      render_data->add_vertex(glm::vec2(left + border_width, bottom - border_width), border_color);
      render_data->add_vertex(glm::vec2(left + border_width, bottom - border_width), border_color);
      render_data->add_vertex(glm::vec2(left, bottom - border_width), border_color);
      render_data->add_vertex(glm::vec2(left, top + border_width), border_color);

      render_data->add_vertex(glm::vec2(right, top + border_width), border_color);
      render_data->add_vertex(glm::vec2(right - border_width, top + border_width), border_color);
      render_data->add_vertex(glm::vec2(right - border_width, bottom - border_width), border_color);
      render_data->add_vertex(glm::vec2(right - border_width, bottom - border_width), border_color);
      render_data->add_vertex(glm::vec2(right, bottom - border_width), border_color);
      render_data->add_vertex(glm::vec2(right, top + border_width), border_color);
    }
  }

  std::unique_ptr<GUIRenderData> render_data;
  std::unique_ptr<GLShaderProgram> shader_program; ///< Rendering program

  // IDs for uniforms.
  GLuint window_size_id;
  GLuint frame_counter_id;

  bool vao_dirty;  ///< Boolean indicating whether VAO needs updating
};

GUIRenderer3D::GUIRenderer3D()
  : impl(new Impl())
{
  name_ = "3D Renderer";
  impl->render_data.reset(new GUIRenderData());
  // Create and compile our GLSL program from the shaders
  impl->shader_program.reset(
    new GLShaderProgram("shaders/GUIVertexShader.glsl",
                        "shaders/GUIFragmentShader.glsl"));

  // Get IDs for uniforms.
  impl->window_size_id = impl->shader_program->get_uniform_id("window_size");
  impl->frame_counter_id = impl->shader_program->get_uniform_id("frame_counter");

  impl->vao_dirty = true;
}

GUIRenderer3D::~GUIRenderer3D()
{

}

bool GUIRenderer3D::visit(GUI& gui)
{
  if (gui.is_dirty())
  {
    gui.clear_dirty();
    impl->vao_dirty = true;
    impl->render_data->clear_vertices();

    return true;
  }
  else
  {
    return false;
  }
}

bool GUIRenderer3D::visit(GUIElement& element)
{
  element.clear_dirty();
  if (element.get_absolute_visibility())
  {
    // TODO: draw
    return true;
  }
  else
  {
    DEEP_TRACE("GUIElement %p is invisible, so ignoring", (void*)&element);
    return false;
  }
}

bool GUIRenderer3D::visit(GUIParentElement& element)
{
  element.clear_dirty();
  if (element.get_absolute_visibility())
  {
    // TODO: draw
    return true;
  }
  else
  {
    DEEP_TRACE("GUIParentElement %p is invisible, so ignoring", (void*)&element);
    return false;
  }
}

bool GUIRenderer3D::visit(GUIFrame& frame)
{
  frame.clear_dirty();
  if (frame.get_absolute_visibility())
  {
    glm::vec2 element_location = frame.getAbsoluteLocation();
    glm::vec2 element_size = frame.getAbsoluteSize();
    DEEP_TRACE("Drawing GUIFrame @ (%f, %f) + (%f, %f)",
               element_location.x, element_location.y,
               element_size.x, element_size.y);
    impl->draw_rect(element_location.x, element_location.x + element_size.x - 1,
                    element_location.y, element_location.y + element_size.y - 1,
                    frame.get_bg_color(), 2, frame.get_fg_color());
    return true;
  }
  else
  {
    DEEP_TRACE("GUIFrame %p is invisible, so ignoring", (void*)&frame);
    return false;
  }
}

bool GUIRenderer3D::visit(GUILabel& label)
{
  TextureFont& font = App::instance().get_fonts().get_default();

  label.clear_dirty();
  if (label.get_absolute_visibility())
  {
    glm::vec2 element_location = label.getAbsoluteLocation();
    glm::vec2 element_size = label.getAbsoluteSize();
    DEEP_TRACE("Drawing GUILabel @ (%f, %f) + (%f, %f)",
               element_location.x, element_location.y,
               element_size.x, element_size.y);
    impl->draw_rect(element_location.x, element_location.x + element_size.x - 1,
                   element_location.y, element_location.y + element_size.y - 1,
                   label.get_bg_color(), 0, label.get_fg_color());

    std::u32string text = label.getText();
    glm::vec2 next_char_coords = glm::vec2(0.0);

    for (unsigned int char_pos = 0; char_pos < text.size(); ++char_pos)
    {
      glm::vec4 char_texture_coords = font.getTextureCoordinates(text[char_pos]);
      glm::vec2 rect_location = glm::vec2(element_location.x + next_char_coords.x,
                                          element_location.y + next_char_coords.y);
      glm::vec2 glyph_size = font.getGlyphSize(text[char_pos]);

      impl->draw_rect(rect_location.x,
                      rect_location.x + glyph_size.x,
                      rect_location.y,
                      rect_location.y + glyph_size.y,
                      label.get_bg_color(), 0, label.get_fg_color(),
                      char_texture_coords, true);

      glm::vec2 char_advance = glm::vec2(font.getXAdvance(text[char_pos]),
                                         font.getLineHeight());

      if (next_char_coords.x + char_advance.x > element_size.x)
      {
        next_char_coords.x = 0;
        next_char_coords.y += char_advance.y;
      }
      else
      {
        next_char_coords.x += char_advance.x;
      }
    }

    return true;
  }
  else
  {
    DEEP_TRACE("GUILabel %p is invisible, so ignoring", (void*)&label);
    return false;
  }
}

void GUIRenderer3D::draw()
{
  sf::Window& window = App::instance().window();
  const sf::Vector2u& window_size = window.getSize();
  TextureFont& font = App::instance().get_fonts().get_default();

  static unsigned int frame_counter;

  // Use our shader program.
  impl->shader_program->bind();

  if (impl->vao_dirty)
  {
    impl->render_data->update_VAO();
    impl->vao_dirty = false;
  }

  // bind texture sampler to renderer.
  font.bind();

  // Send uniforms to renderer.
  glUniform2f(impl->window_size_id, window_size.x, window_size.y);
  glUniform1f(impl->frame_counter_id, frame_counter);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glBindVertexArray(impl->render_data->vao_id);
  glDrawArrays(GL_TRIANGLES, 0, impl->render_data->vertex_count);
  glBindVertexArray(0);

  font.unbind();

  // Unbind our shader program.
  impl->shader_program->unbind();

  ++frame_counter;
}

} // end namespace rectopia
} // end namespace gsl
