// GLShaderProgram.h: Modified from code from www.opengl-tutorial.org.
#ifndef GLSHADERPROGRAM_H_
#define GLSHADERPROGRAM_H_

#include <boost/noncopyable.hpp>
#include <memory>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "common.h"

namespace gsl
{
namespace rectopia
{
  class GLShaderProgram: public boost::noncopyable
  {
    public:
      GLShaderProgram(const char* vertex_file_path, const char* fragment_file_path);
      ~GLShaderProgram();

      GLuint id();

      GLuint getUniformId(const char* variable_name);

      void Bind();
      void Unbind();

    protected:
    private:
      struct Impl;
      // Private implementation pointer
      std::unique_ptr<Impl> impl;
  };


} // end namespace rectopia
} // end namespace gsl

#endif // GLSHADERPROGRAM_H_
