#include "GLTexture.h"
#include "ErrorMacros.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <soil.h>

struct GLTexture::Impl
{
  GLuint texture_id;
};

GLTexture::GLTexture() :
  impl(new Impl())
{
  // Set texture ID to SOIL_CREATE_NEW_ID.
  impl->texture_id = SOIL_CREATE_NEW_ID;
}

GLTexture::~GLTexture()
{
  // Destroy the texture, if it exists.
  if (SOIL_CREATE_NEW_ID != impl->texture_id)
  {
    glDeleteTextures(1, &(impl->texture_id));
  }
}

bool GLTexture::load(char const* filename)
{
  GLuint result = SOIL_load_OGL_texture(filename,
                                        SOIL_LOAD_AUTO,
                                        impl->texture_id,
                                        SOIL_FLAG_POWER_OF_TWO
                                        // | SOIL_FLAG_MIPMAPS
                                        | SOIL_FLAG_TEXTURE_REPEATS
                                        //| SOIL_FLAG_MULTIPLY_ALPHA
                                        //| SOIL_FLAG_COMPRESS_TO_DXT
                                        );

  if (0 == result)
  {
    FATAL_ERROR("SOIL error loading \"%s\": %s", filename, SOIL_last_result());
    return false;
  }

  // Save the resulting texture ID.
  impl->texture_id = result;

  return true;
};

GLuint GLTexture::get_texture_id()
{
  return impl->texture_id;
}

void GLTexture::bind()
{
  if (SOIL_CREATE_NEW_ID != impl->texture_id)
  {
    glBindTexture(GL_TEXTURE_2D, impl->texture_id);
  }
  else
  {
    MINOR_ERROR("Tried to bind a GLTexture that wasn't loaded\n");
  }
}

void GLTexture::unbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}
