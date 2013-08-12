#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "Image.h"

#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

class GLTexture
{
  public:
    GLTexture();
    virtual ~GLTexture();

    bool initialize(unsigned int width_, unsigned int height_);
    bool blit(Image& src_image,
              unsigned int src_width,
              unsigned int src_height,
              unsigned int dst_x_start,
              unsigned int dst_y_start,
              unsigned int src_x_start,
              unsigned int src_y_start);

    void update();

    GLuint get_texture_id();
    void bind();
    void unbind();
  protected:
  private:
    // Private implementation pointer
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // GLTEXTURE_H
