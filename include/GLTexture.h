#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

class GLTexture
{
  public:
    GLTexture();
    virtual ~GLTexture();

    bool load(char const* filename);

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
