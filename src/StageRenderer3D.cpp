#include "StageRenderer3D.h"

#include <algorithm>
#include <iterator>
#include <vector>
#include <boost/container/list.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_set.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Application.h"
#include "ChunkRenderData.h"
#include "ChunkVertexRenderData.h"
#include "CursorRenderData.h"
#include "CursorVertexRenderData.h"
#include "GLShaderProgram.h"
#include "MathUtils.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "StageChunk.h"
#include "StageChunkCollection.h"

namespace gsl
{
namespace rectopia
{

struct StageRenderer3D::Impl
{
  /// Draws the stage block requested.
  void draw_stage_block(StageBlock& block, ChunkRenderData& data)
  {
    static StageCoord3 stage_size = Stage::getInstance().size();
    static glm::vec3 center_coord = glm::vec3(stage_size.x / 2,
                                    stage_size.y / 2,
                                    stage_size.z / 2);

    glm::vec3 coord = glm::vec3(block.get_coords().x,
                                block.get_coords().y,
                                block.get_coords().z);

    FaceBools hiddenFacesSolid = block.get_hidden_faces(BlockLayer::Solid);
    FaceBools hiddenFacesFluid = block.get_hidden_faces(BlockLayer::Fluid);

    glm::vec4 colorSolid = block.get_substance(BlockLayer::Solid).getData().color;
    glm::vec4 colorFluid = block.get_substance(BlockLayer::Fluid).getData().color;

    glm::vec4 colorSpecularSolid = block.get_substance(BlockLayer::Solid).getData().color_specular;
    glm::vec4 colorSpecularFluid = block.get_substance(BlockLayer::Fluid).getData().color_specular;

    if ((Settings::debugMapRevealAll || block.is_known()) &&
        block.is_visible() && block.has_any_visible_faces())
    {
      draw_block(data, coord, center_coord, colorSolid, colorSpecularSolid, hiddenFacesSolid);
      draw_block(data, coord, center_coord, colorFluid, colorSpecularFluid, hiddenFacesFluid);
    }
  }

  /// Draw the cursor.
  void draw_cursor(CursorRenderData& data,
                  glm::vec4 color)
  {
    // Set up the vertex coordinates.  Z and Y are flipped because the game
    // treats Y as the back-to-front coord and Z as the top-to-bottom coord.
    glm::vec3 bkLoLt(0, 0, 0);
    glm::vec3 bkLoRt(1, 0, 0);
    glm::vec3 ftLoLt(0, 0, 1);
    glm::vec3 ftLoRt(1, 0, 1);
    glm::vec3 bkUpRt(1, 1, 0);
    glm::vec3 bkUpLt(0, 1, 0);
    glm::vec3 ftUpRt(1, 1, 1);
    glm::vec3 ftUpLt(0, 1, 1);

    data.add_vertex(bkLoLt, color);
    data.add_vertex(bkLoRt, color);
    data.add_vertex(bkLoRt, color);
    data.add_vertex(bkUpRt, color);
    data.add_vertex(bkUpRt, color);
    data.add_vertex(bkUpLt, color);
    data.add_vertex(bkUpLt, color);
    data.add_vertex(bkLoLt, color);
    data.add_vertex(bkLoLt, color);
    data.add_vertex(ftLoLt, color);
    data.add_vertex(bkLoRt, color);
    data.add_vertex(ftLoRt, color);
    data.add_vertex(bkUpRt, color);
    data.add_vertex(ftUpRt, color);
    data.add_vertex(bkUpLt, color);
    data.add_vertex(ftUpLt, color);
    data.add_vertex(ftLoLt, color);
    data.add_vertex(ftLoRt, color);
    data.add_vertex(ftLoRt, color);
    data.add_vertex(ftUpRt, color);
    data.add_vertex(ftUpRt, color);
    data.add_vertex(ftUpLt, color);
    data.add_vertex(ftUpLt, color);
    data.add_vertex(ftLoLt, color);
  }

  /// Draws a stage block, taking into account hidden faces and shifted top
  /// corners.
  void draw_block(ChunkRenderData& data,
                  glm::vec3 coord,
                  glm::vec3 center,
                  glm::vec4 color,
                  glm::vec4 color_specular,
                  FaceBools hidden = FaceBools())
  {
    // Get the vertex coordinates; Z/Y are flipped because the game treats
    // Y as back-to-front and Z as top-to-bottom.
    float xc = (float)coord.x - (float)center.x;
    float yc = (float)coord.z - (float)center.z;
    float zc = (float)coord.y - (float)center.y;

    if (color.a == 0)
    {
      return;
    }

    static glm::vec2 texCoord(0, 0);

    // Set up the vertex coordinates.  Z and Y are flipped because the game
    // treats Y as the back-to-front coord and Z as the top-to-bottom coord.
    glm::vec3 bkLoLt(xc, yc, zc);
    glm::vec3 bkLoRt(xc + 1.0f, yc, zc);
    glm::vec3 ftLoLt(xc, yc, zc + 1.0f);
    glm::vec3 ftLoRt(xc + 1.0f, yc, zc + 1.0f);
    glm::vec3 bkUpRt(xc + 1.0f, yc + 1.0f, zc);
    glm::vec3 bkUpLt(xc, yc + 1.0f, zc);
    glm::vec3 ftUpRt(xc + 1.0f, yc + 1.0f, zc + 1.0f);
    glm::vec3 ftUpLt(xc, yc + 1.0f, zc + 1.0f);

    static const glm::vec3 point_bk = glm::vec3(0.0f, 0.0f, -1.0f);
    static const glm::vec3 point_fd = glm::vec3(0.0f, 0.0f, 1.0f);
    static const glm::vec3 point_lt = glm::vec3(-1.0f, 0.0f, 0.0f);
    static const glm::vec3 point_rt = glm::vec3(1.0f, 0.0f, 0.0f);
    static const glm::vec3 point_dn = glm::vec3(0.0f, -1.0f, 0.0f);
    static const glm::vec3 point_up = glm::vec3(0.0f, 1.0f, 0.0f);

    if (hidden.back() != true)
    {
      data.add_vertex(coord, bkLoLt, point_bk, color, color_specular, texCoord);
      data.add_vertex(coord, bkLoRt, point_bk, color, color_specular, texCoord);
      data.add_vertex(coord, bkUpRt, point_bk, color, color_specular, texCoord);
      data.add_vertex(coord, bkUpRt, point_bk, color, color_specular, texCoord);
      data.add_vertex(coord, bkUpLt, point_bk, color, color_specular, texCoord);
      data.add_vertex(coord, bkLoLt, point_bk, color, color_specular, texCoord);
    }

    if (hidden.top() != true)
    {
      data.add_vertex(coord, bkUpRt, point_up, color, color_specular, texCoord);
      data.add_vertex(coord, bkUpLt, point_up, color, color_specular, texCoord);
      data.add_vertex(coord, ftUpLt, point_up, color, color_specular, texCoord);
      data.add_vertex(coord, ftUpLt, point_up, color, color_specular, texCoord);
      data.add_vertex(coord, ftUpRt, point_up, color, color_specular, texCoord);
      data.add_vertex(coord, bkUpRt, point_up, color, color_specular, texCoord);
    }

    if (hidden.left() != true)
    {
      data.add_vertex(coord, ftUpLt, point_lt, color, color_specular, texCoord);
      data.add_vertex(coord, bkUpLt, point_lt, color, color_specular, texCoord);
      data.add_vertex(coord, bkLoLt, point_lt, color, color_specular, texCoord);
      data.add_vertex(coord, bkLoLt, point_lt, color, color_specular, texCoord);
      data.add_vertex(coord, ftLoLt, point_lt, color, color_specular, texCoord);
      data.add_vertex(coord, ftUpLt, point_lt, color, color_specular, texCoord);
    }

    if (hidden.bottom() != true)
    {
      data.add_vertex(coord, bkLoRt, point_dn, color, color_specular, texCoord);
      data.add_vertex(coord, bkLoLt, point_dn, color, color_specular, texCoord);
      data.add_vertex(coord, ftLoLt, point_dn, color, color_specular, texCoord);
      data.add_vertex(coord, ftLoLt, point_dn, color, color_specular, texCoord);
      data.add_vertex(coord, ftLoRt, point_dn, color, color_specular, texCoord);
      data.add_vertex(coord, bkLoRt, point_dn, color, color_specular, texCoord);
    }

    if (hidden.right() != true)
    {
      data.add_vertex(coord, ftUpRt, point_rt, color, color_specular, texCoord);
      data.add_vertex(coord, bkUpRt, point_rt, color, color_specular, texCoord);
      data.add_vertex(coord, bkLoRt, point_rt, color, color_specular, texCoord);
      data.add_vertex(coord, bkLoRt, point_rt, color, color_specular, texCoord);
      data.add_vertex(coord, ftLoRt, point_rt, color, color_specular, texCoord);
      data.add_vertex(coord, ftUpRt, point_rt, color, color_specular, texCoord);
    }

    if (hidden.front() != true)
    {
      data.add_vertex(coord, ftLoLt, point_fd, color, color_specular, texCoord);
      data.add_vertex(coord, ftLoRt, point_fd, color, color_specular, texCoord);
      data.add_vertex(coord, ftUpRt, point_fd, color, color_specular, texCoord);
      data.add_vertex(coord, ftUpRt, point_fd, color, color_specular, texCoord);
      data.add_vertex(coord, ftUpLt, point_fd, color, color_specular, texCoord);
      data.add_vertex(coord, ftLoLt, point_fd, color, color_specular, texCoord);
    }
  }

  typedef boost::ptr_map<StageChunk*, ChunkRenderData> ChunkRenderDataMap;
  typedef boost::container::list<StageChunk*> StaleChunkCollection;

  ChunkRenderDataMap chunk_data_;     ///< Map of rendering data to StageChunks
  StaleChunkCollection stale_chunks_; ///< List of chunks that need refreshing

  glm::mat4 projection_matrix_;       ///< Projection matrix
  glm::mat4 view_matrix_;             ///< View matrix

  glm::vec3 light_position_;          ///< Light position in world space
  glm::vec3 light_color_;             ///< Light color
  GLfloat light_power_;               ///< Light power

  GLfloat camera_zoom_;               ///< Camera zoom, from 0 to 100.
  GLfloat last_camera_zoom_;          ///< Previous camera zoom setting

  GLfloat camera_height_;             ///< Camera height, from -90 to 90.
  GLfloat last_camera_height_;        ///< Previous camera height setting

  GLuint frame_counter_;              ///< Frame counter

  StageCoord3 last_cursor_location_;  ///< Previous cursor position

  /// Mutex for stale chunks set.
  boost::mutex stale_chunks_mutex_;

  std::unique_ptr<GLShaderProgram> chunk_program_; ///< Chunk rendering program

  /// IDs for some uniform variables: chunk rendering program.
  struct _chunk_program_id_
  {
    GLuint projection_matrix;
    GLuint view_matrix;
    GLuint model_matrix;
    GLuint light_position;
    GLuint light_color;
    GLuint light_power;
    GLuint cursor_location;
    GLuint frame_counter;
  } chunk_program_id_;

  CursorRenderData cursor_data_;  ///< Cursor rendering data

  std::unique_ptr<GLShaderProgram> cursor_program_; ///< Cursor rendering program

  /// IDs for some uniform variables: cursor rendering program.
  struct _cursor_program_id_
  {
    GLuint projection_matrix;
    GLuint view_matrix;
    GLuint frame_counter;
  } cursor_program_id_;
};

StageRenderer3D::StageRenderer3D()
  : StageRenderer(), impl(new Impl())
{
  name_ = "3D Renderer";

  // Create and compile the GLSL chunk rendering program from the shaders.
  impl->chunk_program_.reset(
    new GLShaderProgram("shaders/ChunkVertexShader.glsl",
                        "shaders/ChunkFragmentShader.glsl"));

  // Get the IDs for the chunk rendering program.
  impl->chunk_program_id_.projection_matrix = impl->chunk_program_->getUniformId("projection_matrix");
  impl->chunk_program_id_.view_matrix = impl->chunk_program_->getUniformId("view_matrix");
  impl->chunk_program_id_.light_position = impl->chunk_program_->getUniformId("light_position_worldspace");
  impl->chunk_program_id_.light_color = impl->chunk_program_->getUniformId("light_color");
  impl->chunk_program_id_.light_power = impl->chunk_program_->getUniformId("light_power");
  impl->chunk_program_id_.cursor_location = impl->chunk_program_->getUniformId("cursor_location");
  impl->chunk_program_id_.frame_counter = impl->chunk_program_->getUniformId("frame_counter");

  // Create and compile the GLSL cursor rendering program from the shaders.
  impl->cursor_program_.reset(
    new GLShaderProgram("shaders/CursorVertexShader.glsl",
                        "shaders/CursorFragmentShader.glsl"));

  // Get the IDs for the cursor rendering program.
  impl->cursor_program_id_.projection_matrix = impl->cursor_program_->getUniformId("projection_matrix");
  impl->cursor_program_id_.view_matrix = impl->cursor_program_->getUniformId("view_matrix");
  impl->cursor_program_id_.frame_counter = impl->cursor_program_->getUniformId("frame_counter");

  // Draw the cursor wireframe.
  impl->draw_cursor(impl->cursor_data_, glm::vec4(1.0));
  impl->cursor_data_.update_VAOs();

  // Initialize the view and projection matrices.
  impl->view_matrix_ = glm::mat4(1.0f);
  impl->projection_matrix_ =
    glm::perspective(45.0f, 16.0f / 9.0f, 0.75f, 500.0f);

  // Initialize the light position (in worldspace), color, and power.
  impl->light_position_ = glm::vec3(0.0f, 0.0f, -50.0f);
  impl->light_color_ = glm::vec3(1.0f);
  impl->light_power_ = 25000.0f; // It IS the sun, after all!

  // Set our camera variables.
  impl->camera_zoom_ = 40.0f;
  impl->camera_height_ = 15.0f;
}

StageRenderer3D::~StageRenderer3D()
{
}

bool StageRenderer3D::visit(Stage& stage)
{
  return true;
}

bool StageRenderer3D::visit(Prop& prop)
{
  return true;
}

bool StageRenderer3D::visit(StageBlock& block)
{
  return true;
}

bool StageRenderer3D::visit(StageChunk& chunk)
{
  // See if the chunk's vertex data needs to be recalculated.
  if (chunk.is_render_data_dirty())
  {
    boost::mutex::scoped_lock lock(impl->stale_chunks_mutex_);

    // Try to avoid pushing the exact same chunk onto the list twice in a row.
    // This will not eliminate all repeat chunks, but it will reduce them.
    // TODO: Another option is to periodically use .unique() to compress the
    //       list.
    if ((impl->stale_chunks_.size() == 0) ||
        (impl->stale_chunks_.back() != &chunk))
    {
      impl->stale_chunks_.push_back(&chunk);
      chunk.set_render_data_dirty(false);
    }
  }

  // Since we visited the blocks underneath ourself, we don't need the caller to do it.
  return false;
}

bool StageRenderer3D::visit(StageChunkCollection& collection)
{
  return true;
}

EventResult StageRenderer3D::handleWindowResize(int w, int h)
{
  // Reset the viewport.
  glViewport(0.0f, 0.0f, (float) w, (float) h);

  // Reset the projection matrix.
  impl->projection_matrix_ =
    glm::perspective(45.0f, (GLfloat) w / (GLfloat) h, 0.75f, 300.0f);

  return EventResult::Handled;
}

EventResult StageRenderer3D::handleKeyDown(sf::Event::KeyEvent key)
{
  // Control+Arrows, Control+<> moves the camera.
  if (key.shift != true && key.control == true && key.alt != true)
  {
    switch (key.code)
    {
    case sf::Keyboard::Up:
      if (impl->camera_height_ < 90)
      {
        impl->camera_height_ += 1;
      }
      return EventResult::Handled;

    case sf::Keyboard::Down:
      if (impl->camera_height_ > -90)
      {
        impl->camera_height_ -= 1;
      }
      return EventResult::Handled;

    case sf::Keyboard::Comma:
      if (impl->camera_zoom_ > 0.0f)
      {
        impl->camera_zoom_ -= 0.25f;
      }
      return EventResult::Handled;

    case sf::Keyboard::Period:
      if (impl->camera_zoom_ < 100.0f)
      {
        impl->camera_zoom_ += 0.25f;
      }
      return EventResult::Handled;

    default:
      return EventResult::Ignored;
    }
  }

  return EventResult::Ignored;
}

void StageRenderer3D::prepare()
{
  sf::Window& window = App::instance().window();
  const sf::Vector2u& windowSize = window.getSize();

  static bool firstPrepare = true;
  if (firstPrepare)
  {
    handleWindowResize(windowSize.x, windowSize.y);
    firstPrepare = false;
  }

  // See if any chunks are stale and need re-rendering.
  {
    unsigned int stale_chunk_count = 16; /// @todo eliminate magic number

    boost::mutex::scoped_lock lock(impl->stale_chunks_mutex_);

    // Update stale chunks on the list, up to the number defined above.
    while ((impl->stale_chunks_.size() > 0) && (stale_chunk_count > 0))
    {
      --stale_chunk_count;
      StageChunk* chunk = impl->stale_chunks_.front();
      impl->stale_chunks_.pop_front();
      ChunkRenderData& render_data = impl->chunk_data_[chunk];

      render_data.clear_vertices();

      StageCoord3 chunk_coords = chunk->get_coords();

      for (StageCoord add_y = 0;
                      add_y < StageChunk::chunk_side_length; ++add_y)
      {
        for (StageCoord add_x = 0;
                        add_x < StageChunk::chunk_side_length; ++add_x)
        {
          int block_x = chunk_coords.x + add_x;
          int block_y = chunk_coords.y + add_y;
          int block_z = chunk_coords.z;

          StageBlock& block = chunk->get_parent()->get_block(block_x,
                                                            block_y,
                                                            block_z);

          impl->draw_stage_block(block, render_data);
        }
      }

      // Update vertex information on the GPU.
      render_data.update_VAOs();
    }
  }
}

void StageRenderer3D::draw()
{
  Stage& stage = Stage::getInstance();

  if (stage.isReady())
  {
    impl->chunk_program_->Bind();

    StageCoord3 stage_size = stage.size();
    StageCoord3 cursor_location = stage.cursor();

    // Figure out the angle to tilt.
    if ((impl->last_cursor_location_ != cursor_location) ||
        (impl->last_camera_zoom_ != impl->camera_zoom_) ||
        (impl->last_camera_height_ != impl->camera_height_))
    {
      // Start view matrix at identity.
      impl->view_matrix_ = glm::mat4(1.0f);

      // Translate to back the camera out.
      impl->view_matrix_ = glm::translate(impl->view_matrix_,
                                          0.0f, 0.0f, -impl->camera_zoom_);

      // Rotate the view.
      impl->view_matrix_ = glm::rotate(impl->view_matrix_,
                                       impl->camera_height_,
                                       1.0f, 0.0f, 0.0f);

      // Translate so the cursor is in the center.
      impl->view_matrix_ = glm::translate(impl->view_matrix_,
                                          glm::vec3((stage_size.x / 2) - cursor_location.x,
                                              (stage_size.z / 2) - cursor_location.z,
                                              (stage_size.y / 2) - cursor_location.y));

      impl->last_cursor_location_ = cursor_location;
      impl->last_camera_zoom_ = impl->camera_zoom_;
      impl->last_camera_height_ = impl->camera_height_;
    }

    // Reposition light if necessary.
    impl->light_position_ = glm::vec3(0.0f, (float)stage_size.z + 32.0f, 0.0f);

    // Send matrices to GLSL.
    glUniformMatrix4fv(impl->chunk_program_id_.projection_matrix, 1, GL_FALSE,
                       &impl->projection_matrix_[0][0]);
    glUniformMatrix4fv(impl->chunk_program_id_.view_matrix, 1, GL_FALSE,
                       &impl->view_matrix_[0][0]);

    // Send light data to GLSL.
    glUniform3f(impl->chunk_program_id_.light_position,
                impl->light_position_.x,
                impl->light_position_.y,
                impl->light_position_.z);
    glUniform3f(impl->chunk_program_id_.light_color,
                impl->light_color_.r,
                impl->light_color_.g,
                impl->light_color_.b);
    glUniform1f(impl->chunk_program_id_.light_power,
                impl->light_power_);

    // Send cursor location to GLSL.
    glUniform3f(impl->chunk_program_id_.cursor_location,
                cursor_location.x,
                cursor_location.y,
                cursor_location.z);

    // Send frame counter to GLSL.
    glUniform1f(impl->chunk_program_id_.frame_counter, impl->frame_counter_);

    // Draw solid vertices.
    for (Impl::ChunkRenderDataMap::iterator data_iterator =
           impl->chunk_data_.begin(); data_iterator != impl->chunk_data_.end();
         ++data_iterator)
    {
      glBindVertexArray(data_iterator->second->solid_vao_id);
      glDrawArrays(GL_TRIANGLES, 0,
                   data_iterator->second->solid_vertex_count);
    }

    // Draw translucent vertices.
    for (Impl::ChunkRenderDataMap::iterator data_iterator =
           impl->chunk_data_.begin(); data_iterator != impl->chunk_data_.end();
         ++data_iterator)
    {
      glBindVertexArray(data_iterator->second->translucent_vao_id);
      glDrawArrays(GL_TRIANGLES, 0,
                   data_iterator->second->translucent_vertex_count);
    }

    glBindVertexArray(0);

    impl->chunk_program_->Unbind();

    // Draw the wireframe cursor.
    // TODO: Instead of solid white, make it pulsate between black and white.

    impl->cursor_program_->Bind();

    // Send uniforms to the cursor program.
    glUniformMatrix4fv(impl->cursor_program_id_.projection_matrix, 1, GL_FALSE,
                       &impl->projection_matrix_[0][0]);
    glUniformMatrix4fv(impl->cursor_program_id_.view_matrix, 1, GL_FALSE,
                       &impl->view_matrix_[0][0]);
    glUniform1f(impl->cursor_program_id_.frame_counter, impl->frame_counter_);

    glBindVertexArray(impl->cursor_data_.vao_id);
    glDrawArrays(GL_LINES, 0, impl->cursor_data_.vertex_count);
    glBindVertexArray(0);

    impl->cursor_program_->Unbind();
  }
}

void StageRenderer3D::finish()
{
  // Increment frame counter.
  ++(impl->frame_counter_);
}

} // namespace rectopia
} // namespace gsl
