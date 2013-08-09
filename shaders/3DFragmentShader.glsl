#version 330 core

in vec4 color_diffuse;
in vec4 color_specular;

in vec3 nml_cameraspace;
in vec4 eye_cameraspace;

out vec4 out_color;

uniform uint lighting_enabled;

uniform vec3 light_color; // not used at the moment
uniform vec3 light_dir_worldspace;

void main()
{
  if (lighting_enabled != 0u)
  {
    vec4 color_ambient = vec4(0.25, 0.25, 0.25, 1.0) * color_diffuse;
    vec4 final_color_specular = vec4(0.0);

    // Normalize vectors.
    vec3 n = normalize(nml_cameraspace);
    vec3 e = normalize(eye_cameraspace.xyz);

    float intensity = max(dot(n, light_dir_worldspace), 0.0);

    if (intensity > 0.0)
    {
      vec3 h = normalize(light_dir_worldspace + e);
      float int_spec = max(dot(h, n), 0.0);
      final_color_specular = color_specular * pow(int_spec, 5);
    }

    // Create the proper color.
    out_color = max(intensity * color_diffuse + final_color_specular,
                    color_ambient);
  }
  else
  {
    // Lighting disabled, just use color_diffuse directly.
    out_color = color_diffuse;
  }
}
