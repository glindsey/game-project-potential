#include "GLTexture.h"
#include "ErrorMacros.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

struct GLTexture::Impl
{
  GLuint texture_id;
  unsigned int width;
  unsigned int height;
  std::unique_ptr<Image> image;

  /// Return the array index for the specified pixel.
  unsigned int pixel(unsigned int x, unsigned int y)
  {
    return (y * width) + x;
  }
};

GLTexture::GLTexture() :
  impl(new Impl())
{
  // Set width, height to zero.
  impl->width = 0;
  impl->height = 0;

  // Generate the texture.
  glGenTextures(1, &(impl->texture_id));

}

GLTexture::~GLTexture()
{
  // Destroy the texture.
  glDeleteTextures(1, &(impl->texture_id));
}

bool GLTexture::initialize(unsigned int width_, unsigned int height_)
{
  // Reset the texture data array.
  impl->image.reset(new Image(width_, height_));
  return true;
}

bool GLTexture::blit(Image& src_image,
                     unsigned int src_width,
                     unsigned int src_height,
                     unsigned int dst_x_start = 0,
                     unsigned int dst_y_start = 0,
                     unsigned int src_x_start = 0,
                     unsigned int src_y_start = 0)
{
  for (unsigned int y_offset = 0; y_offset < src_height; ++y_offset)
  {
    for (unsigned int x_offset = 0; x_offset < src_width; ++x_offset)
    {
      unsigned int src_x = src_x_start + x_offset;
      unsigned int src_y = src_y_start + y_offset;
      unsigned int dst_x = dst_x_start + x_offset;
      unsigned int dst_y = dst_y_start + y_offset;

      impl->image->set_pixel(dst_x, dst_y, src_image.get_pixel(src_x, src_y));
    }
  }
  return true;
};

void GLTexture::update()
{
  // Upload the underlying image to OpenGL.
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               impl->width,
               impl->height,
               0,
               GL_RGBA,
               GL_UNSIGNED_INT_8_8_8_8,
               impl->image->get_data_ptr());
}

GLuint GLTexture::get_texture_id()
{
  return impl->texture_id;
}

void GLTexture::bind()
{
  glBindTexture(GL_TEXTURE_2D, impl->texture_id);
}

void GLTexture::unbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}
