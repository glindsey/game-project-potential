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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "Application.h"
#include "ErrorMacros.h"
#include "GLShaderProgram.h"
#include "GLTexture.h"
#include "MathUtils.h"
#include "RenderData.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "StageChunk.h"
#include "StageChunkCollection.h"
#include "SubstanceLibrary.h"
#include "VertexRenderData.h"

struct StageRenderer3D::Impl
{
  /// Draws the stage block requested.
  void draw_stage_block(StageBlock& block, RenderData& data)
  {
    glm::vec3 coord = glm::vec3(block.get_coords().x,
                                block.get_coords().y,
                                block.get_coords().z);

    FaceBools hiddenFacesSolid = block.get_hidden_faces(BlockLayer::Solid);
    FaceBools hiddenFacesFluid = block.get_hidden_faces(BlockLayer::Fluid);

    glm::vec4 colorSolid = SL->get(block.get_substance(BlockLayer::Solid))->get_data().color;
    glm::vec4 colorFluid = SL->get(block.get_substance(BlockLayer::Fluid))->get_data().color;

    glm::vec4 colorSpecularSolid = SL->get(block.get_substance(BlockLayer::Solid))->get_data().color_specular;
    glm::vec4 colorSpecularFluid = SL->get(block.get_substance(BlockLayer::Fluid))->get_data().color_specular;

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

    // Texture coordinates
    static glm::vec2 texCoord(0, 0);
    static glm::vec2 txLoLt(0, 0);
    static glm::vec2 txLoRt(1, 0);
    static glm::vec2 txUpLt(0, 1);
    static glm::vec2 txUpRt(1, 1);

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
    // The vertices form an octagon as follows:
    //
    //     1---2     |
    //    /     \    |
    //   8       3   |
    //   |   C   |   +Z
    //   7       4   |
    //    \     /    |
    //     6---5     v
    //
    //  --- +X --->

    glm::vec3 CapLo1(xc + 0.4, yc + 1.0, zc + 0.2);
    glm::vec3 CapLo2(xc + 0.6, yc + 1.0, zc + 0.2);
    glm::vec3 CapLo3(xc + 0.8, yc + 1.0, zc + 0.4);
    glm::vec3 CapLo4(xc + 0.8, yc + 1.0, zc + 0.6);
    glm::vec3 CapLo5(xc + 0.6, yc + 1.0, zc + 0.8);
    glm::vec3 CapLo6(xc + 0.4, yc + 1.0, zc + 0.8);
    glm::vec3 CapLo7(xc + 0.2, yc + 1.0, zc + 0.6);
    glm::vec3 CapLo8(xc + 0.2, yc + 1.0, zc + 0.4);
    glm::vec3 CapHi1(xc + 0.4, yc + 1.2, zc + 0.2);
    glm::vec3 CapHi2(xc + 0.6, yc + 1.2, zc + 0.2);
    glm::vec3 CapHi3(xc + 0.8, yc + 1.2, zc + 0.4);
    glm::vec3 CapHi4(xc + 0.8, yc + 1.2, zc + 0.6);
    glm::vec3 CapHi5(xc + 0.6, yc + 1.2, zc + 0.8);
    glm::vec3 CapHi6(xc + 0.4, yc + 1.2, zc + 0.8);
    glm::vec3 CapHi7(xc + 0.2, yc + 1.2, zc + 0.6);
    glm::vec3 CapHi8(xc + 0.2, yc + 1.2, zc + 0.4);
    glm::vec3 CapHiC(xc + 0.5, yc + 1.2, zc + 0.5);

    // Pointer vectors for normals.
    static glm::vec3 const point_N   = glm::vec3( 0.0f,  0.0f, -1.0f);
    static glm::vec3 const point_NNE = glm::vec3( 0.5f,  0.0f, -1.0f);
    static glm::vec3 const point_NE  = glm::vec3( 1.0f,  0.0f, -1.0f);
    static glm::vec3 const point_ENE = glm::vec3( 1.0f,  0.0f, -0.5f);
    static glm::vec3 const point_E   = glm::vec3( 1.0f,  0.0f,  0.0f);
    static glm::vec3 const point_ESE = glm::vec3( 1.0f,  0.0f,  0.5f);
    static glm::vec3 const point_SE  = glm::vec3( 1.0f,  0.0f,  1.0f);
    static glm::vec3 const point_SSE = glm::vec3( 0.5f,  0.0f,  1.0f);
    static glm::vec3 const point_S   = glm::vec3( 0.0f,  0.0f,  1.0f);
    static glm::vec3 const point_SSW = glm::vec3(-0.5f,  0.0f,  1.0f);
    static glm::vec3 const point_SW  = glm::vec3(-1.0f,  0.0f,  1.0f);
    static glm::vec3 const point_WSW = glm::vec3(-1.0f,  0.0f,  0.5f);
    static glm::vec3 const point_W   = glm::vec3(-1.0f,  0.0f,  0.0f);
    static glm::vec3 const point_WNW = glm::vec3(-1.0f,  0.0f, -0.5f);
    static glm::vec3 const point_NW  = glm::vec3(-1.0f,  0.0f, -1.0f);
    static glm::vec3 const point_NNW = glm::vec3(-0.5f,  0.0f, -1.0f);

    static glm::vec3 const point_U   = glm::vec3( 0.0f,  1.0f,  0.0f);
    static glm::vec3 const point_D   = glm::vec3( 0.0f, -1.0f,  0.0f);

    if (hidden.back() != true)
    {
      data.add_vertex(coord, bkLoLt, point_N, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoRt, point_N, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpRt, point_N, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpRt, point_N, color, color_spec, texCoord);
      data.add_vertex(coord, bkUpLt, point_N, color, color_spec, texCoord);
      data.add_vertex(coord, bkLoLt, point_N, color, color_spec, texCoord);
    }

    if (hidden.top() != true)
    {
      // Top
      data.add_vertex(coord, bkUpRt, point_U, color, color_spec, txUpRt);
      data.add_vertex(coord, bkUpLt, point_U, color, color_spec, txUpLt);
      data.add_vertex(coord, ftUpLt, point_U, color, color_spec, txLoLt);
      data.add_vertex(coord, ftUpLt, point_U, color, color_spec, txLoLt);
      data.add_vertex(coord, ftUpRt, point_U, color, color_spec, txLoRt);
      data.add_vertex(coord, bkUpRt, point_U, color, color_spec, txUpRt);

      // Cap N
      data.add_vertex(coord, CapLo1, point_NNW, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo2, point_NNE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi2, point_NNE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi2, point_NNE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi1, point_NNW, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo1, point_NNW, color, color_spec, texCoord);

      // Cap NE
      data.add_vertex(coord, CapLo2, point_NNE, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo3, point_ENE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi3, point_ENE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi3, point_ENE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi2, point_NNE, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo2, point_NNE, color, color_spec, texCoord);

      // Cap E
      data.add_vertex(coord, CapLo3, point_ENE, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo4, point_ESE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi4, point_ESE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi4, point_ESE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi3, point_ENE, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo3, point_ENE, color, color_spec, texCoord);

      // Cap SE
      data.add_vertex(coord, CapLo4, point_ESE, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo5, point_SSE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi5, point_SSE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi5, point_SSE, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi4, point_ESE, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo4, point_ESE, color, color_spec, texCoord);

      // Cap S
      data.add_vertex(coord, CapLo5, point_SSE, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo6, point_SSW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi6, point_SSW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi6, point_SSW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi5, point_SSE, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo5, point_SSE, color, color_spec, texCoord);

      // Cap SW
      data.add_vertex(coord, CapLo6, point_SSW, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo7, point_WSW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi7, point_WSW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi7, point_WSW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi6, point_SSW, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo6, point_SSW, color, color_spec, texCoord);

      // Cap W
      data.add_vertex(coord, CapLo7, point_WSW, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo8, point_WNW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi8, point_WNW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi8, point_WNW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi7, point_WSW, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo7, point_WSW, color, color_spec, texCoord);

      // Cap NW
      data.add_vertex(coord, CapLo8, point_WNW, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo1, point_NNW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi1, point_NNW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi1, point_NNW, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi8, point_WNW, color, color_spec, texCoord);
      data.add_vertex(coord, CapLo8, point_WNW, color, color_spec, texCoord);

      // Cap top
      data.add_vertex(coord, CapHi1, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi2, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHiC, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi2, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi3, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHiC, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi3, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi4, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHiC, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi4, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi5, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHiC, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi5, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi6, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHiC, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi6, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi7, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHiC, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi7, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi8, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHiC, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi8, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHi1, point_U, color, color_spec, texCoord);
      data.add_vertex(coord, CapHiC, point_U, color, color_spec, texCoord);
    }

    if (hidden.left() != true)
    {
      data.add_vertex(coord, ftUpLt, point_W, color, color_spec, txUpRt);
      data.add_vertex(coord, bkUpLt, point_W, color, color_spec, txUpLt);
      data.add_vertex(coord, bkLoLt, point_W, color, color_spec, txLoLt);
      data.add_vertex(coord, bkLoLt, point_W, color, color_spec, txLoLt);
      data.add_vertex(coord, ftLoLt, point_W, color, color_spec, txLoRt);
      data.add_vertex(coord, ftUpLt, point_W, color, color_spec, txUpRt);
    }

    if (hidden.bottom() != true)
    {
      data.add_vertex(coord, bkLoRt, point_D, color, color_spec, txLoRt);
      data.add_vertex(coord, bkLoLt, point_D, color, color_spec, txLoLt);
      data.add_vertex(coord, ftLoLt, point_D, color, color_spec, txUpLt);
      data.add_vertex(coord, ftLoLt, point_D, color, color_spec, txUpLt);
      data.add_vertex(coord, ftLoRt, point_D, color, color_spec, txUpRt);
      data.add_vertex(coord, bkLoRt, point_D, color, color_spec, txLoRt);
    }

    if (hidden.right() != true)
    {
      data.add_vertex(coord, ftUpRt, point_E, color, color_spec, txUpLt);
      data.add_vertex(coord, bkUpRt, point_E, color, color_spec, txUpRt);
      data.add_vertex(coord, bkLoRt, point_E, color, color_spec, txLoRt);
      data.add_vertex(coord, bkLoRt, point_E, color, color_spec, txLoRt);
      data.add_vertex(coord, ftLoRt, point_E, color, color_spec, txLoLt);
      data.add_vertex(coord, ftUpRt, point_E, color, color_spec, txUpLt);
    }

    if (hidden.front() != true)
    {
      data.add_vertex(coord, ftLoLt, point_S, color, color_spec, txLoLt);
      data.add_vertex(coord, ftLoRt, point_S, color, color_spec, txLoRt);
      data.add_vertex(coord, ftUpRt, point_S, color, color_spec, txUpRt);
      data.add_vertex(coord, ftUpRt, point_S, color, color_spec, txUpRt);
      data.add_vertex(coord, ftUpLt, point_S, color, color_spec, txUpLt);
      data.add_vertex(coord, ftLoLt, point_S, color, color_spec, txLoLt);
    }
  }

  typedef boost::ptr_map<StageChunk*, RenderData> RenderDataMap;
  typedef std::list<StageChunk*> StaleChunkCollection;

  RenderDataMap chunk_data;         ///< Map of rendering data to StageChunks
  StaleChunkCollection stale_chunks_; ///< List of chunks that need refreshing

  glm::vec3 light_dir;              ///< Light direction in world space
  glm::vec3 light_color;            ///< Light color

  float angle_of_view;            ///< Current angle of view, in degrees
  float camera_zoom;              ///< Camera zoom, from -100 to 100.
  float camera_x_angle;           ///< Camera X angle, from -30 to 30.
  float camera_y_angle;           ///< Camera Y angle, from -90 to 90.

  GLuint frame_counter;             ///< Frame counter

  boost::mutex stale_chunks_mutex; ///< Mutex for stale chunks set.

  std::unique_ptr<GLShaderProgram> render_program; ///< Chunk rendering program

  /// IDs for some uniform variables: chunk rendering program.
  struct _render_program_id_
  {
    GLuint m_matrix;
    GLuint v_matrix;
    GLuint p_matrix;

    GLuint light_dir;
    GLuint light_color;

    GLuint cursor_location;
    GLuint frame_counter;
    GLuint lighting_enabled;
    GLuint pulse_color;

    GLuint texture_unit;

  } render_program_id;

  RenderData cursor_data;  ///< Cursor rendering data

  std::unique_ptr<GLShaderProgram> cursor_program; ///< Cursor rendering program

  /// TEST CODE: Test checkerboard texture.
  //std::unique_ptr<GLTexture> texture_test;
  //sf::Texture texture_test;
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

  program_id.m_matrix =         program.get_uniform_id("m_matrix");
  program_id.v_matrix =         program.get_uniform_id("v_matrix");
  program_id.p_matrix =         program.get_uniform_id("p_matrix");

  program_id.light_dir =        program.get_uniform_id("light_dir_worldspace");
  program_id.light_color =      program.get_uniform_id("light_color");

  program_id.cursor_location =  program.get_uniform_id("cursor_location");
  program_id.frame_counter =    program.get_uniform_id("frame_counter");
  program_id.lighting_enabled = program.get_uniform_id("lighting_enabled");
  program_id.pulse_color =      program.get_uniform_id("pulse_color");

  program_id.texture_unit =     program.get_uniform_id("texture_sampler");

  // Draw the cursor wireframe.
  impl->draw_cursor(impl->cursor_data,
                    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
                    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  impl->cursor_data.update_VAOs();

  // Initialize the light direction and color.
  impl->light_dir = glm::vec3(-0.25f, 1.0f, -0.25f);
  impl->light_color = glm::vec3(1.0f);

  // Set our camera variables.
  impl->angle_of_view = 45.0f;
  impl->camera_zoom = -30.0f;
  impl->camera_x_angle = -10.0f;
  impl->camera_y_angle = 30.0f;

  // TEST CODE: Load the test texture.
  //impl->texture_test.reset(new GLTexture());
  //impl->texture_test->load("textures/test-checkerboard.png");

  // Bind the shader's texture sampler to unit #0.
  glUniform1i(impl->render_program_id.texture_unit, 0);
  glActiveTexture(GL_TEXTURE0);
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
  StageShPtr stage = Stage::get_instance();

  // Set the viewport to match window size.
  glViewport(0.0f, 0.0f, (float) window_size.x, (float) window_size.y);

  // See if any chunks are stale and need re-rendering.
  {
    unsigned int stale_chunk_count = 1; /// @todo eliminate magic number

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

  if (stage->is_ready())
  {
    impl->render_program->bind();

    StageCoord3 cursor_location = stage->cursor();
    glm::vec3& ldir = impl->light_dir;
    glm::vec3& lcol = impl->light_color;
    StageCoord3& cloc = cursor_location;

    glm::mat4 m_matrix;
    glm::mat4 v_matrix;
    glm::mat4 p_matrix;

    m_matrix = glm::translate(-cloc.x, -cloc.z, -cloc.y);

    v_matrix = glm::mat4(1.0f);
    v_matrix = glm::rotate(impl->camera_x_angle, glm::vec3(0.0, 1.0, 0.0)) * v_matrix;
    v_matrix = glm::rotate(impl->camera_y_angle, glm::vec3(1.0, 0.0, 0.0)) * v_matrix;
    v_matrix = glm::translate(glm::vec3(0, 0, impl->camera_zoom)) * v_matrix;

    p_matrix = glm::perspective(impl->angle_of_view,
                                (float)window_size.x / (float)window_size.y,
                                0.75f, 100.0f);

    glUniformMatrix4fv(impl->render_program_id.m_matrix, 1, GL_FALSE, glm::value_ptr(m_matrix));
    glUniformMatrix4fv(impl->render_program_id.v_matrix, 1, GL_FALSE, glm::value_ptr(v_matrix));
    glUniformMatrix4fv(impl->render_program_id.p_matrix, 1, GL_FALSE, glm::value_ptr(p_matrix));

    glUniform3f(impl->render_program_id.light_dir, ldir.x, ldir.y, ldir.z);
    glUniform3f(impl->render_program_id.light_color, lcol.r, lcol.g, lcol.b);
    glUniform1ui(impl->render_program_id.lighting_enabled, 1);
    glUniform1ui(impl->render_program_id.pulse_color, 0);
    glUniform3f(impl->render_program_id.cursor_location, cloc.x, cloc.y, cloc.z);
    glUniform1ui(impl->render_program_id.frame_counter, impl->frame_counter);

    // TEST CODE: Bind the test texture.
    //impl->texture_test->bind();

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

    // TEST CODE: Unbind the test texture.
    //impl->texture_test->unbind();

    // Draw the wireframe cursor.

    // Send uniforms to the program.
    m_matrix = glm::mat4(1.0);
    glUniformMatrix4fv(impl->render_program_id.m_matrix, 1, GL_FALSE, glm::value_ptr(m_matrix));

    glUniform1ui(impl->render_program_id.lighting_enabled, 0);
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
