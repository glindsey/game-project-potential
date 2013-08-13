// GLShaderProgram.h: Modified from code from www.opengl-tutorial.org.
#ifndef GLSHADERPROGRAM_H_
#define GLSHADERPROGRAM_H_

#include <boost/noncopyable.hpp>
#include <memory>
#include <GL/glew.h>

#include "common.h"

class GLShaderProgram: public boost::noncopyable
{
  public:
    GLShaderProgram(const char* vertex_file_path, const char* fragment_file_path);
    ~GLShaderProgram();

    GLuint id();

    GLuint get_uniform_id(const char* variable_name);

    void bind();
    void unbind();

  protected:
  private:
    struct Impl;
    // Private implementation pointer
    std::unique_ptr<Impl> impl;
};

#endif // GLSHADERPROGRAM_H_
