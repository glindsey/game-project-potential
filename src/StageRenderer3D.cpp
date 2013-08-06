#include "StageRenderer3D.h"

#include <algorithm>
#include <iterator>
#include <list>
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
      draw_block(data, coord, colorSolid, colorSpecularSolid, hiddenFacesSolid);
      draw_block(data, coord, colorFluid, colorSpecularFluid, hiddenFacesFluid);
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

  /// Draws a stage block, taking into account hidden faces.
  void draw_block(ChunkRenderData& data,
                  glm::vec3 coord,
                  glm::vec4 color,
                  glm::vec4 color_spec,
                  FaceBools hidden = FaceBools())
  {
    // Get the vertex coordinates; Z/Y are flipped because the game treats
    // Y as back-to-front and Z as top-to-bottom.
    float xc = (float)coord.x;
    float yc = (float)coord.z;
    float zc = (float)coord.y;

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

    // Coordinates to make a little cap on top of the block.
    glm::vec3 CapBkLoRt(xc + 0.8f, yc + 1.0f, zc + 0.2f);
    glm::vec3 CapBkLoLt(xc + 0.2f, yc + 1.0f, zc + 0.2f);
    glm::vec3 CapFtLoRt(xc + 0.8f, yc + 1.0f, zc + 0.8f);
    glm::vec3 CapFtLoLt(xc + 0.2f, yc + 1.0f, zc + 0.8f);
    glm::vec3 CapBkUpRt(xc + 0.8f, yc + 1.2f, zc + 0.2f);
    glm::vec3 CapBkUpLt(xc + 0.2f, yc + 1.2f, zc + 0.2f);
    glm::vec3 CapFtUpRt(xc + 0.8f, yc + 1.2f, zc + 0.8f);
    glm::vec3 CapFtUpLt(xc + 0.2f, yc + 1.2f, zc + 0.8f);

    // Coordinates to make a little dimple on bottom of the block.
    glm::vec3 DplBkLoRt(xc + 0.8f, yc + 0.0f, zc + 0.2f);
    glm::vec3 DplBkLoLt(xc + 0.2f, yc + 0.0f, zc + 0.2f);
    glm::vec3 DplFtLoRt(xc + 0.8f, yc + 0.0f, zc + 0.8f);
    glm::vec3 DplFtLoLt(xc + 0.2f, yc + 0.0f, zc + 0.8f);
    glm::vec3 DplBkUpRt(xc + 0.8f, yc + 0.2f, zc + 0.2f);
    glm::vec3 DplBkUpLt(xc + 0.2f, yc + 0.2f, zc + 0.2f);
    glm::vec3 DplFtUpRt(xc + 0.8f, yc + 0.2f, zc + 0.8f);
    glm::vec3 DplFtUpLt(xc + 0.2f, yc + 0.2f, zc + 0.8f);

    static const glm::vec3 point_bk = glm::vec3(0.0f, 0.0f, -1.0f);
    static const glm::vec3 point_fd = glm::vec3(0.0f, 0.0f, 1.0f);
    static const glm::vec3 point_lt = glm::vec3(-1.0f, 0.0f, 0.0f);
    static const glm::vec3 point_rt = glm::vec3(1.0f, 0.0f, 0.0f);
    static const glm::vec3 point_dn = glm::vec3(0.0f, -1.0f, 0.0f);
    static const glm::vec3 point_up = glm::vec3(0.0f, 1.0f, 0.0f);

    if (hidden.back() != true)
    {
      data.add_vertex(coord, bkLoLt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoRt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpRt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpRt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpLt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoLt, point_bk, color, color_spec, texCoord);
    }

    if (hidden.top() != true)
    {
      // Top back
      data.add_vertex(coord, bkUpRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpRt, point_up, color, color_spec, texCoord);

      // Top left
      data.add_vertex(coord, bkUpLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, ftUpLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpLt, point_up, color, color_spec, texCoord);

      // Top right
      data.add_vertex(coord, bkUpRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, ftUpRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpRt, point_up, color, color_spec, texCoord);

      // Top front
      data.add_vertex(coord, ftUpRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, ftUpLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, ftUpRt, point_up, color, color_spec, texCoord);

      // Cap back
      data.add_vertex(coord, CapBkLoLt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoRt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkUpRt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkUpRt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkUpLt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoLt, point_bk, color, color_spec, texCoord);

      // Cap left
      data.add_vertex(coord, CapFtUpLt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkUpLt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoLt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoLt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoLt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtUpLt, point_lt, color, color_spec, texCoord);

      // Cap right
      data.add_vertex(coord, CapFtUpRt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkUpRt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoRt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkLoRt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoRt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtUpRt, point_rt, color, color_spec, texCoord);

      // Cap front
      data.add_vertex(coord, CapFtLoLt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoRt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtUpRt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtUpRt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtUpLt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtLoLt, point_fd, color, color_spec, texCoord);

      // Cap top
      data.add_vertex(coord, CapBkUpRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkUpLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtUpLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtUpLt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapFtUpRt, point_up, color, color_spec, texCoord);
      data.add_vertex(coord, CapBkUpRt, point_up, color, color_spec, texCoord);
    }

    if (hidden.left() != true)
    {
      data.add_vertex(coord, ftUpLt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpLt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoLt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoLt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, ftLoLt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, ftUpLt, point_lt, color, color_spec, texCoord);
    }

    if (hidden.bottom() != true)
    {
      // Bottom back
      data.add_vertex(coord, bkLoRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoRt, point_dn, color, color_spec, texCoord);

      // Bottom left
      data.add_vertex(coord, bkLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, ftLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoLt, point_dn, color, color_spec, texCoord);

      // Bottom right
      data.add_vertex(coord, bkLoRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, ftLoRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoRt, point_dn, color, color_spec, texCoord);

      // Bottom front
      data.add_vertex(coord, ftLoRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, ftLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, ftLoRt, point_dn, color, color_spec, texCoord);

      // Dimple back
      data.add_vertex(coord, DplBkLoLt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoRt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkUpRt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkUpRt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkUpLt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoLt, point_fd, color, color_spec, texCoord);

      // Dimple left
      data.add_vertex(coord, DplFtUpLt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkUpLt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoLt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoLt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoLt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtUpLt, point_rt, color, color_spec, texCoord);

      // Dimple right
      data.add_vertex(coord, DplFtUpRt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkUpRt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoRt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkLoRt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoRt, point_lt, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtUpRt, point_lt, color, color_spec, texCoord);

      // Dimple front
      data.add_vertex(coord, DplFtLoLt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoRt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtUpRt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtUpRt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtUpLt, point_bk, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtLoLt, point_bk, color, color_spec, texCoord);

      // Dimple top
      data.add_vertex(coord, DplBkUpRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkUpLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtUpLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtUpLt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplFtUpRt, point_dn, color, color_spec, texCoord);
      data.add_vertex(coord, DplBkUpRt, point_dn, color, color_spec, texCoord);

      //data.add_vertex(coord, bkLoRt, point_dn, color, color_spec, texCoord);
      //data.add_vertex(coord, bkLoLt, point_dn, color, color_spec, texCoord);
      //data.add_vertex(coord, ftLoLt, point_dn, color, color_spec, texCoord);
      //data.add_vertex(coord, ftLoLt, point_dn, color, color_spec, texCoord);
      //data.add_vertex(coord, ftLoRt, point_dn, color, color_spec, texCoord);
      //data.add_vertex(coord, bkLoRt, point_dn, color, color_spec, texCoord);
    }

    if (hidden.right() != true)
    {
      data.add_vertex(coord, ftUpRt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpRt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoRt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoRt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, ftLoRt, point_rt, color, color_spec, texCoord);
      data.add_vertex(coord, ftUpRt, point_rt, color, color_spec, texCoord);
    }

    if (hidden.front() != true)
    {
      data.add_vertex(coord, ftLoLt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, ftLoRt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, ftUpRt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, ftUpRt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, ftUpLt, point_fd, color, color_spec, texCoord);
      data.add_vertex(coord, ftLoLt, point_fd, color, color_spec, texCoord);
    }
  }

  typedef boost::ptr_map<StageChunk*, ChunkRenderData> ChunkRenderDataMap;
  typedef std::list<StageChunk*> StaleChunkCollection;

  ChunkRenderDataMap chunk_data;     ///< Map of rendering data to StageChunks
  StaleChunkCollection stale_chunks_; ///< List of chunks that need refreshing

  glm::vec3 light_dir;               ///< Light direction in world space
  glm::vec3 light_color;             ///< Light color

  GLfloat angle_of_view;              ///< Current angle of view, in degrees
  GLfloat camera_zoom;               ///< Camera zoom, from -100 to 100.
  GLfloat camera_x_angle;             ///< Camera X angle, from -30 to 30.
  GLfloat camera_y_angle;             ///< Camera Y angle, from -90 to 90.

  GLuint frame_counter;              ///< Frame counter

  /// Mutex for stale chunks set.
  boost::mutex stale_chunks_mutex_;

  std::unique_ptr<GLShaderProgram> chunk_program; ///< Chunk rendering program

  /// IDs for some uniform variables: chunk rendering program.
  struct _chunk_program_id_
  {
    GLuint light_dir;
    GLuint light_color;
    GLuint cursor_location;
    GLuint frame_counter;

    GLuint angle_of_view;
    GLuint aspect_ratio;
    GLuint z_near;
    GLuint z_far;
    GLuint camera_zoom;
    GLuint camera_x_angle;
    GLuint camera_y_angle;
  } chunk_program_id;

  CursorRenderData cursor_data;  ///< Cursor rendering data

  std::unique_ptr<GLShaderProgram> cursor_program; ///< Cursor rendering program

  /// IDs for some uniform variables: cursor rendering program.
  struct _cursor_program_id_
  {
    GLuint cursor_location;
    GLuint frame_counter;
  } cursor_program_id;
};

StageRenderer3D::StageRenderer3D()
  : StageRenderer(), impl(new Impl())
{
  name_ = "3D Renderer";

  // Create and compile the GLSL chunk rendering program from the shaders.
  impl->chunk_program.reset(
    new GLShaderProgram("shaders/ChunkVertexShader.glsl",
                        "shaders/ChunkFragmentShader.glsl"));

  // Get the IDs for the chunk rendering program.
  impl->chunk_program_id.light_dir = impl->chunk_program->get_uniform_id("light_dir_worldspace");
  impl->chunk_program_id.light_color = impl->chunk_program->get_uniform_id("light_color");

  impl->chunk_program_id.cursor_location = impl->chunk_program->get_uniform_id("cursor_location");
  impl->chunk_program_id.frame_counter = impl->chunk_program->get_uniform_id("frame_counter");

  impl->chunk_program_id.angle_of_view = impl->chunk_program->get_uniform_id("angle_of_view");
  impl->chunk_program_id.aspect_ratio = impl->chunk_program->get_uniform_id("aspect_ratio");
  impl->chunk_program_id.z_near = impl->chunk_program->get_uniform_id("z_near");
  impl->chunk_program_id.z_far = impl->chunk_program->get_uniform_id("z_far");
  impl->chunk_program_id.camera_zoom = impl->chunk_program->get_uniform_id("camera_zoom");
  impl->chunk_program_id.camera_x_angle = impl->chunk_program->get_uniform_id("camera_x_angle");
  impl->chunk_program_id.camera_y_angle = impl->chunk_program->get_uniform_id("camera_y_angle");

  // Create and compile the GLSL cursor rendering program from the shaders.
  impl->cursor_program.reset(
    new GLShaderProgram("shaders/CursorVertexShader.glsl",
                        "shaders/CursorFragmentShader.glsl"));

  // Get the IDs for the cursor rendering program.
  impl->cursor_program_id.cursor_location = impl->chunk_program->get_uniform_id("cursor_location");
  impl->cursor_program_id.frame_counter = impl->cursor_program->get_uniform_id("frame_counter");

  // Draw the cursor wireframe.
  impl->draw_cursor(impl->cursor_data, glm::vec4(1.0));
  impl->cursor_data.update_VAOs();

  // Initialize the light direction and color.
  impl->light_dir = glm::vec3(0.0f, 1.0f, 0.0f);
  impl->light_color = glm::vec3(1.0f);

  // Set our camera variables.
  impl->angle_of_view = 45.0f;
  impl->camera_zoom = -50.0f;
  impl->camera_x_angle = 0.0f;
  impl->camera_y_angle = 20.0f;
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
      if (impl->camera_y_angle < 90)
      {
        impl->camera_y_angle += 1;
        std::cout << "Camera Y angle now " << impl->camera_y_angle << std::endl;
      }
      return EventResult::Handled;

    case sf::Keyboard::Down:
      if (impl->camera_y_angle > -90)
      {
        impl->camera_y_angle -= 1;
        std::cout << "Camera Y angle now " << impl->camera_y_angle << std::endl;
      }
      return EventResult::Handled;

    case sf::Keyboard::Left:
      if (impl->camera_x_angle < 45)
      {
        impl->camera_x_angle += 1;
        std::cout << "Camera X angle now " << impl->camera_x_angle << std::endl;
      }
      return EventResult::Handled;

    case sf::Keyboard::Right:
      if (impl->camera_x_angle > -45)
      {
        impl->camera_x_angle -= 1;
        std::cout << "Camera X angle now " << impl->camera_x_angle << std::endl;
      }
      return EventResult::Handled;

    case sf::Keyboard::Comma:
      if (impl->camera_zoom > -100.0f)
      {
        impl->camera_zoom -= 0.5f;
        std::cout << "Camera zoom now " << impl->camera_zoom << std::endl;
      }
      return EventResult::Handled;

    case sf::Keyboard::Period:
      if (impl->camera_zoom < 100.0f)
      {
        impl->camera_zoom += 0.5f;
        std::cout << "Camera zoom now " << impl->camera_zoom << std::endl;
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
  const sf::Vector2u& window_size = window.getSize();

  static bool firstPrepare = true;
  if (firstPrepare)
  {
    handleWindowResize(window_size.x, window_size.y);
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
      ChunkRenderData& render_data = impl->chunk_data[chunk];

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
  sf::Window& window = App::instance().window();
  const sf::Vector2u& window_size = window.getSize();

  Stage& stage = Stage::getInstance();

  if (stage.isReady())
  {
    impl->chunk_program->bind();

    StageCoord3 stage_size = stage.size();
    StageCoord3 cursor_location = stage.cursor();

    // Send camera data to GLSL.
    glUniform1f(impl->chunk_program_id.angle_of_view, impl->angle_of_view);
    glUniform1f(impl->chunk_program_id.aspect_ratio, (GLfloat) window_size.x /
                                                     (GLfloat) window_size.y);
    glUniform1f(impl->chunk_program_id.z_near, 0.75f);
    glUniform1f(impl->chunk_program_id.z_far, 300.0f);
    glUniform1f(impl->chunk_program_id.camera_zoom, impl->camera_zoom);
    glUniform1f(impl->chunk_program_id.camera_x_angle, impl->camera_x_angle);
    glUniform1f(impl->chunk_program_id.camera_y_angle, impl->camera_y_angle);

    // Send light data to GLSL.
    glm::vec3& ldir = impl->light_dir;
    glm::vec3& lcol = impl->light_color;
    glUniform3f(impl->chunk_program_id.light_dir, ldir.x, ldir.y, ldir.z);
    glUniform3f(impl->chunk_program_id.light_color, lcol.r, lcol.g, lcol.b);

    // Send cursor location to GLSL.
    StageCoord3& cloc = cursor_location;
    glUniform3f(impl->chunk_program_id.cursor_location, cloc.x, cloc.y, cloc.z);

    // Send frame counter to GLSL.
    glUniform1f(impl->chunk_program_id.frame_counter, impl->frame_counter);

    // Draw solid vertices.
    for (Impl::ChunkRenderDataMap::iterator data_iterator =
           impl->chunk_data.begin(); data_iterator != impl->chunk_data.end();
         ++data_iterator)
    {
      ChunkRenderData* chunk = data_iterator->second;
      glBindVertexArray(chunk->solid_vao_id);
      glDrawArrays(GL_TRIANGLES, 0, chunk->solid_vertex_count);
    }

    // Draw translucent vertices.
    for (Impl::ChunkRenderDataMap::iterator data_iterator =
           impl->chunk_data.begin(); data_iterator != impl->chunk_data.end();
         ++data_iterator)
    {
      ChunkRenderData* chunk = data_iterator->second;
      glBindVertexArray(chunk->translucent_vao_id);
      glDrawArrays(GL_TRIANGLES, 0, chunk->translucent_vertex_count);
    }

    glBindVertexArray(0);

    impl->chunk_program->unbind();

/*
    // Draw the wireframe cursor.
    // TODO: Instead of solid white, make it pulsate between black and white.

    impl->cursor_program->bind();

    // Send uniforms to the cursor program.
    glUniformMatrix4fv(impl->cursor_program_id.projection_matrix, 1, GL_FALSE,
                       &impl->projection_matrix_[0][0]);
    glUniformMatrix4fv(impl->cursor_program_id.view_matrix, 1, GL_FALSE,
                       &impl->view_matrix[0][0]);
    glUniform3f(impl->cursor_program_id.cursor_location,
                cursor_location.x,
                cursor_location.y,
                cursor_location.z);
    glUniform1f(impl->cursor_program_id.frame_counter, impl->frame_counter);

    glBindVertexArray(impl->cursor_data.vao_id);
    glDrawArrays(GL_LINES, 0, impl->cursor_data.vertex_count);
    glBindVertexArray(0);

    impl->cursor_program->unbind();
*/
  }
}

void StageRenderer3D::finish()
{
  // Increment frame counter.
  ++(impl->frame_counter);
}

} // namespace rectopia
} // namespace gsl
