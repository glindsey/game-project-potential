#version 330 core

in vec4 pass_color;

out vec4 out_color;

void main()
{
  // Super simple, since there's no lighting for the wireframe cursor.
	out_color = pass_color;
}
