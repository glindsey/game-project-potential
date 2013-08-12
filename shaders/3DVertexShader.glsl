#version 330 core

#define M_PI 3.1415926535897932384626433832795

// Things to remember about geometry in OpenGL:
// MODEL_SPACE: Coordinates relative to the center of the model.
// MODEL_MATRIX: Matrix that translates, rotates and/or scales the model.
// MODEL_MATRIX * MODEL_SPACE_VECTOR = WORLD_SPACE_VECTOR
// WORLD_SPACE: Coordinates relative to the center of the world.
// VIEW_MATRIX: Matrix that translates, rotates and/or scales the world.
// VIEW_MATRIX * WORLD_SPACE_VECTOR = EYE_SPACE_VECTOR
// EYE_SPACE: Coordinates relative to the camera (which is at the origin).
// PROJECTION_MATRIX: Matrix that creates an orthographic or perspective view.
// PROJECTION_MATRIX * CAMERA_SPACE_VECTOR = Final view presented to screen.

layout (location = 0) in vec3 in_block_coords;
layout (location = 1) in vec3 in_pos_modelspace;
layout (location = 2) in vec4 in_color;
layout (location = 3) in vec4 in_color_specular;
layout (location = 4) in vec3 in_normal_modelspace;

out vec4 color_diffuse;
out vec4 color_specular;

out vec3 nml_eyespace;
out vec4 eye_eyespace;

uniform mat4 m_matrix, v_matrix, p_matrix;

uniform vec3 cursor_location;

out vec4 block_normalized;
out vec4 cursor_normalized;

uniform uint lighting_enabled;
uniform uint pulse_color;

uniform uint frame_counter;

const vec3 zero_vector = vec3(0, 0, 0);

mat4 view_frustum(float angle_of_view,
                  float aspect,
                  float z_near,
                  float z_far)
{
  float d2r = M_PI / 180.0;
  float y_scale = 1 / tan(d2r * angle_of_view / 2);
  float x_scale = y_scale / aspect;
  float diff = z_near - z_far;

  return mat4(
      vec4(x_scale,     0.0,                         0.0,  0.0),
      vec4(0.0,     y_scale,                         0.0,  0.0),
      vec4(0.0,         0.0,     (z_far + z_near) / diff, -1.0),
      vec4(0.0,         0.0, (2 * z_far * z_near) / diff,  0.0)
  );
}

mat4 scale(float x, float y, float z)
{
    return mat4(
        vec4(x,   0.0, 0.0, 0.0),
        vec4(0.0, y,   0.0, 0.0),
        vec4(0.0, 0.0, z,   0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
}

mat4 translate(float x, float y, float z)
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(x,   y,   z,   1.0)
    );
}

mat4 rotate_x(float theta_deg)
{
  float theta = radians(theta_deg);
  return mat4(
      vec4(1.0,         0.0,         0.0, 0.0),
      vec4(0.0,  cos(theta), -sin(theta), 0.0),
      vec4(0.0,  sin(theta),  cos(theta), 0.0),
      vec4(0.0,         0.0,         0.0, 1.0)
  );
}

mat4 rotate_y(float theta_deg)
{
  float theta = radians(theta_deg);
  return mat4(
      vec4( cos(theta), 0.0, sin(theta), 0.0),
      vec4(0.0,         1.0,        0.0, 0.0),
      vec4(-sin(theta), 0.0, cos(theta), 0.0),
      vec4(0.0,         0.0,        0.0, 1.0)
  );
}

void main()
{
  mat4 vm_matrix = v_matrix * m_matrix;
  mat4 pvm_matrix = p_matrix * vm_matrix;

  // Position of the vertex in eye space.
  vec4 pos_eyespace = vm_matrix * vec4(in_pos_modelspace, 1.0);

  // Output position of the vertex, in normalized space
  gl_Position = pvm_matrix * vec4(in_pos_modelspace, 1.0);

  block_normalized  = pvm_matrix * vec4(in_pos_modelspace, 1.0);
  cursor_normalized = p_matrix * v_matrix * vec4(0.0, 0.0, 0.0, 1.0);

  // Calculate normal matrix
  mat3 normal_matrix = transpose(inverse(mat3(vm_matrix)));

  // Normal of the the vertex, in camera space
  nml_eyespace = normalize(normal_matrix * in_normal_modelspace);

  // Vector that goes from the vertex to the camera, in camera space.
  eye_eyespace = -(vm_matrix * pos_eyespace);

  if (lighting_enabled != 0u)
  {
    // On-axis calculation.
    vec3 gnomon_color = ((in_block_coords.x == cursor_location.x) ||
                         (in_block_coords.y == cursor_location.y) ||
                         (in_block_coords.z == cursor_location.z)) ?
                           vec3(0.25) : vec3(0.0);

    // Alpha -- if above the cursor, alpha is dropped down to 25%.
    float alpha_adjustment = 1.0;
    /*
    if (in_block_coords.z > cursor_location.z)
    {
      float fade_amt = (float(in_block_coords.z) -
                            float(cursor_location.z)) / 100.0;

      alpha_adjustment = 0.25 - clamp(fade_amt, 0, 0.25);
    }
    */

    // Diffuse color passed to the fragment shader.
    color_diffuse = vec4(in_color.r + gnomon_color.r,
                         in_color.g + gnomon_color.g,
                         in_color.b + gnomon_color.b,
                         in_color.a * alpha_adjustment);

    // Specular color passed to the fragment shader.
    color_specular.rgb = in_color_specular.rgb;
    color_specular.a = in_color_specular.a * alpha_adjustment;
  }
  else
  {
    if (pulse_color != 0u)
    {
      // Get modulo of frame counter to do pulsing features.
      // (Is modulo faster, or would a bitwise AND be better?)
      int frame_counter_modulo = int(frame_counter & 63u);

      if (frame_counter_modulo >= 32)
      {
        frame_counter_modulo = 63 - frame_counter_modulo;
      }

      float fraction = float(frame_counter_modulo) / 32.0;

      color_diffuse = (in_color * fraction) +
                      (in_color_specular * (1 - fraction));
      color_specular = vec4(0.0);
    }
    else
    {
      // Lighting disabled, just use color_diffuse directly.
      color_diffuse = in_color;
      // Specular color passed to the fragment shader.
      color_specular = in_color_specular;
    }
  }
}
