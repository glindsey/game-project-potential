#version 330 core

in vec4 color_diffuse;
in vec4 color_specular;

in vec3 nml_eyespace;
in vec4 eye_eyespace;

in vec4 block_normalized;
in vec4 cursor_normalized;

in vec2 texture_uv;

out vec4 out_color;

uniform uint lighting_enabled;

uniform sampler2D texture_sampler;

uniform vec3 light_color; // not used at the moment
uniform vec3 light_dir_worldspace;

void main()
{
  if (lighting_enabled != 0u)
  {
    // Multiply color_diffuse by the texture_sampler rgb.
    vec4 final_color_diffuse = color_diffuse;
    final_color_diffuse.rgb += texture(texture_sampler, texture_uv).rgb;

    vec4 color_ambient = vec4(0.25, 0.25, 0.25, 1.0) * final_color_diffuse;

    vec4 final_color_specular = vec4(0.0);

    // Normalize vectors.
    vec3 n = normalize(nml_eyespace);
    vec3 e = normalize(eye_eyespace.xyz);

    float intensity = max(dot(n, light_dir_worldspace), 0.0);

    if (intensity > 0.0)
    {
      vec3 h = normalize(light_dir_worldspace + e);
      float int_spec = max(dot(h, n), 0.0);
      final_color_specular = color_specular * pow(int_spec, 5);
    }

    // Create the proper color.
    out_color = max(intensity * final_color_diffuse + final_color_specular,
                    color_ambient);

    // "Porthole" effect: if vertex_normalized.z is greater
    //       than cursor_normalized.z, and we're in a specified diameter,
    //       discard the fragment.
    float v_distance = distance(block_normalized.xy / block_normalized.w,
                                vec2(0.0));
    float z_level = (2 - block_normalized.z) / (block_normalized.w * 2);

    if ((block_normalized.z < cursor_normalized.z) && (v_distance <= 0.5))
    {
      discard;
    }
  }
  else
  {
    // Lighting disabled, just use color_diffuse directly.
    out_color = color_diffuse;
  }
}
