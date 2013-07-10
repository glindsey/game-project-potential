#version 330 core

in vec4 pass_color;
in vec2 pass_textureUV;
in float pass_textured;

out vec4 out_color;

uniform uint frame_counter;
uniform sampler2D texture_sampler;

void main()
{
  vec4 texture_color = texture(texture_sampler, pass_textureUV);

  if (pass_textured > 0.5)
  {
    // The out color's RGB is based on the texture alpha.
    // Right now the R/G/B channels of the texture are not used.
    out_color = pass_color * texture_color.a;
  }
  else
  {
    out_color = pass_color;
  }

  // This might not be needed; keeping it in for safety.
  out_color = clamp(out_color, 0, 1);
}
