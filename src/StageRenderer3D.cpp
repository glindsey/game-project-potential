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
#include "GLShaderProgram.h"
#include "MathUtils.h"
#include "RenderData.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "StageChunk.h"
#include "StageChunkCollection.h"
#include "VertexRenderData.h"

namespace gsl
{
namespace rectopia
{

struct StageRenderer3D::Impl
{
  /// Draws the stage block requested.
  void draw_stage_block(StageBlock& block, RenderData& data)
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
  void draw_cursor(RenderData& data, glm::vec4 color1, glm::vec4 color2)
  {
    // Set up the vertex coordinates.  Z and Y are flipped because the game
    // treats Y as the back-to-front coord and Z as the top-to-bottom coord.
    glm::vec3 farLt(-3, 0.5, 0.5);
    glm::vec3 farRt(4, 0.5, 0.5);
    glm::vec3 farLo(0.5, -3, 0.5);
    glm::vec3 farUp(0.5, 4, 0.5);
    glm::vec3 farBk(0.5, 0.5, -3);
    glm::vec3 farFt(0.5, 0.5, 4);
    glm::vec3 bkLoLt(0, 0, 0);
    glm::vec3 bkLoRt(1, 0, 0);
    glm::vec3 ftLoLt(0, 0, 1);
    glm::vec3 ftLoRt(1, 0, 1);
    glm::vec3 bkUpRt(1, 1, 0);
    glm::vec3 bkUpLt(0, 1, 0);
    glm::vec3 ftUpRt(1, 1, 1);
    glm::vec3 ftUpLt(0, 1, 1);

    static glm::vec2 const dummy2(0, 0);
    static glm::vec3 const dummy3(0, 0, 0);

    // Gnomon
    data.add_vertex(dummy3, farLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, farRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, farLo, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, farUp, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, farBk, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, farFt, dummy3, color1, color2, dummy2);

    // Wireframe block
    data.add_vertex(dummy3, bkLoLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkLoRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkLoRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkUpRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkUpRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkUpLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkUpLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkLoLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkLoLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftLoLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkLoRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftLoRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkUpRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftUpRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, bkUpLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftUpLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftLoLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftLoRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftLoRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftUpRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftUpRt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftUpLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftUpLt, dummy3, color1, color2, dummy2);
    data.add_vertex(dummy3, ftLoLt, dummy3, color1, color2, dummy2);
  }

  /// Draws a stage block, taking into account hidden faces.
  void draw_block(RenderData& data,
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

    static float const bump = 0.25f;   /// @todo move magic number
    static float const b_ht = 0.25f;   /// @todo move magic number

    // Coordinates to make a little cap on top of the block.
    glm::vec3 CapBkLoRt(xc + 1.0f - bump, yc + 1.0f, zc + 0.0f + bump);
    glm::vec3 CapBkLoLt(xc + 0.0f + bump, yc + 1.0f, zc + 0.0f + bump);
    glm::vec3 CapFtLoRt(xc + 1.0f - bump, yc + 1.0f, zc + 1.0f - bump);
    glm::vec3 CapFtLoLt(xc + 0.0f + bump, yc + 1.0f, zc + 1.0f - bump);
    glm::vec3 CapBkUpRt(xc + 1.0f - bump, yc + 1.0f + b_ht, zc + 0.0f + bump);
    glm::vec3 CapBkUpLt(xc + 0.0f + bump, yc + 1.0f + b_ht, zc + 0.0f + bump);
    glm::vec3 CapFtUpRt(xc + 1.0f - bump, yc + 1.0f + b_ht, zc + 1.0f - bump);
    glm::vec3 CapFtUpLt(xc + 0.0f + bump, yc + 1.0f + b_ht, zc + 1.0f - bump);

    // Coordinates to make a little dimple on bottom of the block.
    glm::vec3 DplBkLoRt(xc + 1.0f - bump, yc + 0.0f, zc + 0.0f + bump);
    glm::vec3 DplBkLoLt(xc + 0.0f + bump, yc + 0.0f, zc + 0.0f + bump);
    glm::vec3 DplFtLoRt(xc + 1.0f - bump, yc + 0.0f, zc + 1.0f - bump);
    glm::vec3 DplFtLoLt(xc + 0.0f + bump, yc + 0.0f, zc + 1.0f - bump);
    glm::vec3 DplBkUpRt(xc + 1.0f - bump, yc + 0.0f + b_ht, zc + 0.0f + bump);
    glm::vec3 DplBkUpLt(xc + 0.0f + bump, yc + 0.0f + b_ht, zc + 0.0f + bump);
    glm::vec3 DplFtUpRt(xc + 1.0f - bump, yc + 0.0f + b_ht, zc + 1.0f - bump);
    glm::vec3 DplFtUpLt(xc + 0.0f + bump, yc + 0.0f + b_ht, zc + 1.0f - bump);

    static glm::vec3 const point_bk = glm::vec3( 0.0f,  0.0f, -1.0f);
    static glm::vec3 const point_fd = glm::vec3( 0.0f,  0.0f,  1.0f);
    static glm::vec3 const point_lt = glm::vec3(-1.0f,  0.0f,  0.0f);
    static glm::vec3 const point_rt = glm::vec3( 1.0f,  0.0f,  0.0f);
    static glm::vec3 const point_dn = glm::vec3( 0.0f, -1.0f,  0.0f);
    static glm::vec3 const point_up = glm::vec3( 0.0f,  1.0f,  0.0f);

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

  typedef boost::ptr_map<StageChunk*, RenderData> RenderDataMap;
  typedef std::list<StageChunk*> StaleChunkCollection;

  RenderDataMap chunk_data;         ///< Map of rendering data to StageChunks
  StaleChunkCollection stale_chunks_; ///< List of chunks that need refreshing

  glm::vec3 light_dir;              ///< Light direction in world space
  glm::vec3 light_color;            ///< Light color

  GLfloat angle_of_view;            ///< Current angle of view, in degrees
  GLfloat camera_zoom;              ///< Camera zoom, from -100 to 100.
  GLfloat camera_x_angle;           ///< Camera X angle, from -30 to 30.
  GLfloat camera_y_angle;           ///< Camera Y angle, from -90 to 90.

  GLuint frame_counter;             ///< Frame counter

  boost::mutex stale_chunks_mutex; ///< Mutex for stale chunks set.

  std::unique_ptr<GLShaderProgram> render_program; ///< Chunk rendering program

  /// IDs for some uniform variables: chunk rendering program.
  struct _render_program_id_
  {
    GLuint light_dir;
    GLuint light_color;

    GLuint cursor_location;
    GLuint frame_counter;
    GLuint lighting_enabled;
    GLuint center_on_cursor;
    GLuint pulse_color;

    GLuint angle_of_view;
    GLuint aspect_ratio;
    GLuint z_near;
    GLuint z_far;
    GLuint camera_zoom;
    GLuint camera_x_angle;
    GLuint camera_y_angle;
  } render_program_id;

  RenderData cursor_data;  ///< Cursor rendering data

  std::unique_ptr<GLShaderProgram> cursor_program; ///< Cursor rendering program
};

StageRenderer3D::StageRenderer3D()
  : StageRenderer(), impl(new Impl())
{
  name_ = "3D Renderer";

  // Create and compile the GLSL chunk rendering program from the shaders.
  impl->render_program.reset(
    new GLShaderProgram("shaders/3DVertexShader.glsl",
                        "shaders/3DFragmentShader.glsl"));

  // Get the IDs for the chunk rendering program.
  GLShaderProgram& program = *(impl->render_program.get());
  Impl::_render_program_id_& program_id = impl->render_program_id;

  program_id.light_dir =        program.get_uniform_id("light_dir_worldspace");
  program_id.light_color =      program.get_uniform_id("light_color");

  program_id.cursor_location =  program.get_uniform_id("cursor_location");
  program_id.frame_counter =    program.get_uniform_id("frame_counter");
  program_id.lighting_enabled = program.get_uniform_id("lighting_enabled");
  program_id.center_on_cursor = program.get_uniform_id("center_on_cursor");
  program_id.pulse_color =      program.get_uniform_id("pulse_color");

  program_id.angle_of_view =    program.get_uniform_id("angle_of_view");
  program_id.aspect_ratio =     program.get_uniform_id("aspect_ratio");
  program_id.z_near =           program.get_uniform_id("z_near");
  program_id.z_far =            program.get_uniform_id("z_far");
  program_id.camera_zoom =      program.get_uniform_id("camera_zoom");
  program_id.camera_x_angle =   program.get_uniform_id("camera_x_angle");
  program_id.camera_y_angle =   program.get_uniform_id("camera_y_angle");

  // Draw the cursor wireframe.
  impl->draw_cursor(impl->cursor_data,
                    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
                    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  impl->cursor_data.update_VAOs();

  // Initialize the light direction and color.
  impl->light_dir = glm::vec3(0.0f, 1.0f, 0.0f);
  impl->light_color = glm::vec3(1.0f);

  // Set our camera variables.
  impl->angle_of_view = 45.0f;
  impl->camera_zoom = -30.0f;
  impl->camera_x_angle = -10.0f;
  impl->camera_y_angle = 30.0f;
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
    boost::mutex::scoped_lock lock(impl->stale_chunks_mutex);

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

EventResult StageRenderer3D::handle_window_resize(int w, int h)
{
  // Reset the viewport.
  glViewport(0.0f, 0.0f, (float) w, (float) h);

  return EventResult::Handled;
}

EventResult StageRenderer3D::handle_key_down(sf::Event::KeyEvent key)
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

void StageRenderer3D::draw()
{
  sf::Window& window = App::instance().window();
  const sf::Vector2u& window_size = window.getSize();
  Stage& stage = Stage::getInstance();

  static bool firstPrepare = true;
  if (firstPrepare)
  {
    handle_window_resize(window_size.x, window_size.y);
    firstPrepare = false;
  }

  // See if any chunks are stale and need re-rendering.
  {
    unsigned int stale_chunk_count = 8; /// @todo eliminate magic number

    boost::mutex::scoped_lock lock(impl->stale_chunks_mutex);

    // Update stale chunks on the list, up to the number defined above.
    while ((impl->stale_chunks_.size() > 0) && (stale_chunk_count > 0))
    {
      --stale_chunk_count;
      StageChunk* chunk = impl->stale_chunks_.front();
      impl->stale_chunks_.pop_front();
      RenderData& render_data = impl->chunk_data[chunk];

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

  if (stage.is_ready())
  {
    impl->render_program->bind();

    StageCoord3 stage_size = stage.size();
    StageCoord3 cursor_location = stage.cursor();
    glm::vec3& ldir = impl->light_dir;
    glm::vec3& lcol = impl->light_color;
    StageCoord3& cloc = cursor_location;

    glUniform1f(impl->render_program_id.angle_of_view, impl->angle_of_view);
    glUniform1f(impl->render_program_id.aspect_ratio, (GLfloat) window_size.x /
                                                     (GLfloat) window_size.y);
    glUniform1f(impl->render_program_id.z_near, 0.75f);
    glUniform1f(impl->render_program_id.z_far, 300.0f);
    glUniform1f(impl->render_program_id.camera_zoom, impl->camera_zoom);
    glUniform1f(impl->render_program_id.camera_x_angle, impl->camera_x_angle);
    glUniform1f(impl->render_program_id.camera_y_angle, impl->camera_y_angle);
    glUniform3f(impl->render_program_id.light_dir, ldir.x, ldir.y, ldir.z);
    glUniform3f(impl->render_program_id.light_color, lcol.r, lcol.g, lcol.b);
    glUniform1ui(impl->render_program_id.lighting_enabled, 1);
    glUniform1ui(impl->render_program_id.center_on_cursor, 1);
    glUniform1ui(impl->render_program_id.pulse_color, 0);
    glUniform3f(impl->render_program_id.cursor_location, cloc.x, cloc.y, cloc.z);
    glUniform1ui(impl->render_program_id.frame_counter, impl->frame_counter);

    // Draw solid vertices.
    for (Impl::RenderDataMap::iterator iter = impl->chunk_data.begin();
         iter != impl->chunk_data.end();
         ++iter)
    {
      RenderData* chunk = iter->second;
      glBindVertexArray(chunk->solid_vao_id);
      glDrawArrays(GL_TRIANGLES, 0, chunk->solid_vertex_count);
    }

    // Draw translucent vertices.
    for (Impl::RenderDataMap::iterator iter = impl->chunk_data.begin();
         iter != impl->chunk_data.end();
         ++iter)
    {
      RenderData* chunk = iter->second;
      glBindVertexArray(chunk->translucent_vao_id);
      glDrawArrays(GL_TRIANGLES, 0, chunk->translucent_vertex_count);
    }

    // Draw the wireframe cursor.
    // TODO: Instead of solid white, make it pulsate between black and white.

    // Send uniforms to the program.
    glUniform1ui(impl->render_program_id.lighting_enabled, 0);
    glUniform1ui(impl->render_program_id.center_on_cursor, 0);
    glUniform1ui(impl->render_program_id.pulse_color, 1);

    glBindVertexArray(impl->cursor_data.solid_vao_id);
    glDrawArrays(GL_LINES, 0, impl->cursor_data.solid_vertex_count);
    glBindVertexArray(impl->cursor_data.translucent_vao_id);
    glDrawArrays(GL_LINES, 0, impl->cursor_data.translucent_vertex_count);
    glBindVertexArray(0);

    impl->render_program->unbind();
  }

  // Increment frame counter.
  ++(impl->frame_counter);
}

} // namespace rectopia
} // namespace gsl
