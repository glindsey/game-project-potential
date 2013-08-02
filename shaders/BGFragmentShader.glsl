#version 330 core

in vec4 pass_color;

out vec4 out_color;

uniform uint frame_counter;

void main()
{
  out_color = pass_color;
}
