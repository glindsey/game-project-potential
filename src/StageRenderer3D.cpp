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
#include "BlockTopCorners.h"
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

namespace gsl
{
namespace rectopia
{

struct StageRenderer3D::Impl
{
  /// Draws the stage block requested.
  void drawStageBlock(StageBlock& block, ChunkRenderData& data)
  {
    BlockTopCorners corners = block.top_corners();
    static BlockTopCorners flatCorners;

    static StageCoord3 stage_size = Stage::getInstance().size();
    static glm::vec3 center_coord = glm::vec3(stage_size.x / 2,
                                    stage_size.y / 2,
                                    stage_size.z / 2);

    glm::vec3 coord = glm::vec3(block.getCoordinates().x,
                                block.getCoordinates().y,
                                block.getCoordinates().z);

    FaceBools hiddenFacesSolid = block.getHiddenFaces(BlockLayer::Solid);
    FaceBools hiddenFacesFluid = block.getHiddenFaces(BlockLayer::Fluid);

    glm::vec4 colorSolid = block.substance(BlockLayer::Solid).getData().color;
    glm::vec4 colorFluid = block.substance(BlockLayer::Fluid).getData().color;

    glm::vec4 colorSpecularSolid = block.substance(BlockLayer::Solid).getData().color_specular;
    glm::vec4 colorSpecularFluid = block.substance(BlockLayer::Fluid).getData().color_specular;

    if (block.visible() && block.hasAnyVisibleFaces())
    {
      drawBlock(data, coord, center_coord, colorSolid, colorSpecularSolid, hiddenFacesSolid, corners);
      drawBlock(data, coord, center_coord, colorFluid, colorSpecularFluid, hiddenFacesFluid, flatCorners);
    }
  }

  /// Draw the cursor.
  void drawCursor(CursorRenderData& data,
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

    data.addVertex(bkLoLt, color);
    data.addVertex(bkLoRt, color);
    data.addVertex(bkLoRt, color);
    data.addVertex(bkUpRt, color);
    data.addVertex(bkUpRt, color);
    data.addVertex(bkUpLt, color);
    data.addVertex(bkUpLt, color);
    data.addVertex(bkLoLt, color);
    data.addVertex(bkLoLt, color);
    data.addVertex(ftLoLt, color);
    data.addVertex(bkLoRt, color);
    data.addVertex(ftLoRt, color);
    data.addVertex(bkUpRt, color);
    data.addVertex(ftUpRt, color);
    data.addVertex(bkUpLt, color);
    data.addVertex(ftUpLt, color);
    data.addVertex(ftLoLt, color);
    data.addVertex(ftLoRt, color);
    data.addVertex(ftLoRt, color);
    data.addVertex(ftUpRt, color);
    data.addVertex(ftUpRt, color);
    data.addVertex(ftUpLt, color);
    data.addVertex(ftUpLt, color);
    data.addVertex(ftLoLt, color);
  }

  /// Draws a stage block, taking into account hidden faces and shifted top
  /// corners.
  void drawBlock(ChunkRenderData& data,
                 glm::vec3 coord,
                 glm::vec3 center,
                 glm::vec4 color,
                 glm::vec4 color_specular,
                 FaceBools hidden = FaceBools(),
                 BlockTopCorners corners = BlockTopCorners())
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
    glm::vec3 bkUpRt(xc + 1.0f, yc + 1.0f + (0.5f * corners.back_right()), zc);
    glm::vec3 bkUpLt(xc, yc + 1.0f + (0.5f * corners.back_left()), zc);
    glm::vec3 ftUpRt(xc + 1.0f, yc + 1.0f + (0.5f * corners.front_right()),
                     zc + 1.0f);
    glm::vec3 ftUpLt(xc, yc + 1.0f + (0.5f * corners.front_left()),
                     zc + 1.0f);

    if (hidden.back() != true)
    {
      data.addVertex(coord, bkLoLt, glm::vec3(0.0f, 0.0f, -1.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkLoRt, glm::vec3(0.0f, 0.0f, -1.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkUpRt, glm::vec3(0.0f, 0.0f, -1.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkUpRt, glm::vec3(0.0f, 0.0f, -1.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkUpLt, glm::vec3(0.0f, 0.0f, -1.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkLoLt, glm::vec3(0.0f, 0.0f, -1.0f), color, color_specular, texCoord);
    }

    if (hidden.top() != true)
    {
      // Corner adjustments mean the top triangles may need their normals calculated.
      glm::vec3 topNormal1 = calculateNormal(bkUpRt, bkUpLt, ftUpLt);
      glm::vec3 topNormal2 = calculateNormal(ftUpLt, ftUpRt, bkUpRt);

      data.addVertex(coord, bkUpRt, topNormal1, color, color_specular, texCoord);
      data.addVertex(coord, bkUpLt, topNormal1, color, color_specular, texCoord);
      data.addVertex(coord, ftUpLt, topNormal1, color, color_specular, texCoord);
      data.addVertex(coord, ftUpLt, topNormal2, color, color_specular, texCoord);
      data.addVertex(coord, ftUpRt, topNormal2, color, color_specular, texCoord);
      data.addVertex(coord, bkUpRt, topNormal2, color, color_specular, texCoord);
    }

    if (hidden.left() != true)
    {
      data.addVertex(coord, ftUpLt, glm::vec3(-1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkUpLt, glm::vec3(-1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkLoLt, glm::vec3(-1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkLoLt, glm::vec3(-1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftLoLt, glm::vec3(-1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftUpLt, glm::vec3(-1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
    }

    if (hidden.bottom() != true)
    {
      data.addVertex(coord, bkLoRt, glm::vec3(0.0f, -1.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkLoLt, glm::vec3(0.0f, -1.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftLoLt, glm::vec3(0.0f, -1.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftLoLt, glm::vec3(0.0f, -1.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftLoRt, glm::vec3(0.0f, -1.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkLoRt, glm::vec3(0.0f, -1.0f, 0.0f), color, color_specular, texCoord);
    }

    if (hidden.right() != true)
    {
      data.addVertex(coord, ftUpRt, glm::vec3(1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkUpRt, glm::vec3(1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkLoRt, glm::vec3(1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, bkLoRt, glm::vec3(1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftLoRt, glm::vec3(1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftUpRt, glm::vec3(1.0f, 0.0f, 0.0f), color, color_specular, texCoord);
    }

    if (hidden.front() != true)
    {
      data.addVertex(coord, ftLoLt, glm::vec3(0.0f, 0.0f, 1.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftLoRt, glm::vec3(0.0f, 0.0f, 1.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftUpRt, glm::vec3(0.0f, 0.0f, 1.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftUpRt, glm::vec3(0.0f, 0.0f, 1.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftUpLt, glm::vec3(0.0f, 0.0f, 1.0f), color, color_specular, texCoord);
      data.addVertex(coord, ftLoLt, glm::vec3(0.0f, 0.0f, 1.0f), color, color_specular, texCoord);
    }
  }

  typedef boost::ptr_map<StageChunk*, ChunkRenderData> ChunkRenderDataMap;
  typedef boost::container::list<StageChunk*> StaleChunkCollection;

  ChunkRenderDataMap chunk_data_;     ///< Map of rendering data to StageChunks
  StaleChunkCollection stale_chunks_; ///< List of chunks that need refreshing

  glm::mat4 projection_matrix_;       ///< Projection matrix
  glm::mat4 view_matrix_;             ///< View matrix
  glm::mat4 model_matrix_;            ///< Model matrix

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

  boost::scoped_ptr<GLShaderProgram> chunk_program_; ///< Chunk rendering program

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

  boost::scoped_ptr<GLShaderProgram> cursor_program_; ///< Cursor rendering program

  /// IDs for some uniform variables: cursor rendering program.
  struct _cursor_program_id_
  {
    GLuint projection_matrix;
    GLuint view_matrix;
    GLuint model_matrix;
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
  impl->chunk_program_id_.model_matrix = impl->chunk_program_->getUniformId("model_matrix");
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
  impl->cursor_program_id_.model_matrix = impl->cursor_program_->getUniformId("model_matrix");
  impl->cursor_program_id_.frame_counter = impl->cursor_program_->getUniformId("frame_counter");

  // Draw the cursor wireframe.
  impl->drawCursor(impl->cursor_data_, glm::vec4(1.0));
  impl->cursor_data_.updateVAOs();

  // Initialize the model, view, and projection matrices.
  impl->model_matrix_ = glm::mat4(1.0f);
  impl->view_matrix_ = glm::mat4(1.0f);
  impl->projection_matrix_ =
    glm::perspective(45.0f, 16.0f / 9.0f, 0.75f, 300.0f);

  // Initialize the light position (in worldspace), color, and power.
  impl->light_position_ = glm::vec3(0.0f, 0.0f, -50.0f);
  impl->light_color_ = glm::vec3(1.0f);
  impl->light_power_ = 10000.0f; // It IS the sun, after all!

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
  if (chunk.isRenderDataDirty())
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
      chunk.setRenderDataDirty(false);
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
    boost::mutex::scoped_lock lock(impl->stale_chunks_mutex_);

    // Update the top stale chunk on the list.
    if (impl->stale_chunks_.size() > 0)
    {
      StageChunk* chunk = impl->stale_chunks_.front();
      impl->stale_chunks_.pop_front();
      ChunkRenderData& render_data = impl->chunk_data_[chunk];

      render_data.clearVertices();

      for (int idx = 0; idx < StageChunk::ChunkSize; ++idx)
      {
        StageBlock& block = chunk->getBlock(idx);
        impl->drawStageBlock(block, render_data);
      }

      // Update vertex information on the GPU.
      render_data.updateVAOs();
    }
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void StageRenderer3D::draw()
{
  Stage& stage = Stage::getInstance();

  if (stage.isReady())
  {
    impl->chunk_program_->Bind();

    StageCoord3 stage_size = stage.size();
    StageCoord3 cursor_location = stage.cursor();

    // Model matrix: Remains identity, because the model IS the world here!
    //impl->model_matrix_ = glm::mat4(1.0f);

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

    // Send model matrix to GLSL.
    glUniformMatrix4fv(impl->chunk_program_id_.model_matrix, 1, GL_FALSE,
                       &impl->model_matrix_[0][0]);
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
    glUniformMatrix4fv(impl->cursor_program_id_.model_matrix, 1, GL_FALSE,
                       &impl->model_matrix_[0][0]);
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
