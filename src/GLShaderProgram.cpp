// GLShaderProgram.cpp: Modified from code from www.opengl-tutorial.org.
#include "GLShaderProgram.h"

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "ErrorMacros.h"

namespace gsl
{
namespace rectopia
{

struct GLShaderProgram::Impl
{
  // GL Program ID.
  GLuint id_;
};

GLShaderProgram::GLShaderProgram(char const* vertex_file_path,
                                 char const* fragment_file_path)
  : impl(new Impl())
{
  // Create the shaders
  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the Vertex Shader code from the file
  std::string vertex_shader_code;
  std::ifstream vertex_shader_stream(vertex_file_path, std::ios::in);
  if(vertex_shader_stream.is_open())
  {
    std::string Line = "";
    while(getline(vertex_shader_stream, Line))
    {
      vertex_shader_code += "\n" + Line;
    }
    vertex_shader_stream.close();
  }

  // Read the Fragment Shader code from the file
  std::string fragment_shader_code;
  std::ifstream fragment_shader_stream(fragment_file_path, std::ios::in);
  if(fragment_shader_stream.is_open())
  {
    std::string Line = "";
    while(getline(fragment_shader_stream, Line))
    {
      fragment_shader_code += "\n" + Line;
    }
    fragment_shader_stream.close();
  }

  GLint result_code = GL_FALSE;
  int info_log_length;

  // Compile Vertex Shader
  //printf("Compiling shader : %s\n", vertex_file_path);
  char const* vertex_source_pointer = vertex_shader_code.c_str();
  glShaderSource(vertex_shader_id, 1, &vertex_source_pointer , NULL);
  glCompileShader(vertex_shader_id);

  // Check Vertex Shader
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result_code);
  glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
  std::vector<char> vertex_shader_error_message(info_log_length);
  glGetShaderInfoLog(vertex_shader_id, info_log_length, NULL, &vertex_shader_error_message[0]);
  fprintf(stdout, "%s\n", &vertex_shader_error_message[0]);

  if (result_code == GL_FALSE)
  {
    FATAL_ERROR("GLSL Vertex Shader failed to compile");
  }

  // Compile Fragment Shader
  //printf("Compiling shader : %s\n", fragment_file_path);
  char const* fragment_source_pointer = fragment_shader_code.c_str();
  glShaderSource(fragment_shader_id, 1, &fragment_source_pointer , NULL);
  glCompileShader(fragment_shader_id);

  // Check Fragment Shader
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result_code);
  glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
  std::vector<char> fragment_shader_error_message(info_log_length);
  glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL, &fragment_shader_error_message[0]);
  fprintf(stdout, "%s\n", &fragment_shader_error_message[0]);

  if (result_code == GL_FALSE)
  {
    FATAL_ERROR("GLSL Vertex Shader failed to compile");
  }

  // Link the program
  //fprintf(stdout, "Linking program\n");
  impl->id_ = glCreateProgram();
  glAttachShader(impl->id_, vertex_shader_id);
  glAttachShader(impl->id_, fragment_shader_id);
  glLinkProgram(impl->id_);

  // Check the program
  glGetProgramiv(impl->id_, GL_LINK_STATUS, &result_code);
  glGetProgramiv(impl->id_, GL_INFO_LOG_LENGTH, &info_log_length);
  std::vector<char> program_error_message( std::max(info_log_length, int(1)) );
  glGetProgramInfoLog(impl->id_, info_log_length, NULL, &program_error_message[0]);
  fprintf(stdout, "%s\n", &program_error_message[0]);

  if (result_code == GL_FALSE)
  {
    FATAL_ERROR("GLSL Shader Program failed to link");
  }

  // Delete the individual shaders as they are no longer needed.
  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);
}

GLShaderProgram::~GLShaderProgram()
{
  glDeleteProgram(impl->id_);
}

GLuint GLShaderProgram::id()
{
  return impl->id_;
}

GLuint GLShaderProgram::getUniformId(const char* variable_name)
{
  return glGetUniformLocation(impl->id_, variable_name);
}

void GLShaderProgram::Bind()
{
  glUseProgram(impl->id_);
}

void GLShaderProgram::Unbind()
{
  glUseProgram(0);
}

} // end namespace rectopia
} // end namespace gsl
