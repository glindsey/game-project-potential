#version 330 core

// Things to remember about geometry in OpenGL:
// MODEL_SPACE: Coordinates relative to the center of the model.
// MODEL_MATRIX: Matrix that translates, rotates and/or scales the model.
// MODEL_MATRIX * MODEL_SPACE_VECTOR = WORLD_SPACE_VECTOR
// WORLD_SPACE: Coordinates relative to the center of the world.
// VIEW_MATRIX: Matrix that translates, rotates and/or scales the world.
// VIEW_MATRIX * WORLD_SPACE_VECTOR = CAMERA_SPACE_VECTOR
// CAMERA_SPACE: Coordinates relative to the camera (which is at the origin).
// PROJECTION_MATRIX: Matrix that creates an orthographic or perspective view.
// PROJECTION_MATRIX * CAMERA_SPACE_VECTOR = Final view presented to screen.

layout (location = 0) in vec3 in_block_coordinates;
layout (location = 1) in vec3 in_position_modelspace;
layout (location = 2) in vec4 in_color;
layout (location = 3) in vec4 in_color_specular;
layout (location = 4) in vec3 in_normal_modelspace;

out vec4 pass_color_diffuse;
out vec4 pass_color_specular;
out vec3 pass_position_worldspace;
out vec3 pass_normal_cameraspace;
out vec3 pass_eye_direction_cameraspace;
out vec3 pass_light_direction_cameraspace;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

uniform vec3 light_position_worldspace;
uniform vec3 cursor_location;

uniform uint frame_counter;

const vec3 zero_vector = vec3(0, 0, 0);

void main()
{
    // Output position of the vertex, in clip space
    gl_Position = projection_matrix * view_matrix *
                  vec4(in_position_modelspace, 1.0);

    // Position of the vertex, in worldspace
    pass_position_worldspace = vec4(in_position_modelspace, 1.0).xyz;

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 vertex_position_cameraspace = ( view_matrix *
                                         vec4(pass_position_worldspace, 1.0)).xyz;
    pass_eye_direction_cameraspace = vec3(0, 0, 0) - vertex_position_cameraspace;

    // Vector that goes from the vertex to the light, in camera space.
    vec3 light_position_cameraspace = ( view_matrix *
                                        vec4(light_position_worldspace, 1.0)).xyz;
    pass_light_direction_cameraspace = light_position_cameraspace +
                                       pass_eye_direction_cameraspace;

    // Normal of the the vertex, in camera space
    pass_normal_cameraspace = ( view_matrix *
                                vec4(in_normal_modelspace, 0.0)).xyz;

    // Get modulo of frame counter to do pulsing features.
    // (Is modulo faster, or would a bitwise AND be better?)
    uint frame_counter_modulo = frame_counter % 256u;

    // On-axis calculation.
    vec3 gnomon_color = ((in_block_coordinates.x == cursor_location.x) ||
                         (in_block_coordinates.y == cursor_location.y)) ?
                           vec3(0.5) : vec3(0.0);

    // Cursor distance
    float cursor_highlight = 0.5 - clamp(pow(distance(in_block_coordinates.xy,
                                                       cursor_location.xy),
                                             5) / 500, 0, 0.5);
    vec3 cursor_color = vec3(cursor_highlight);

    // Alpha -- if above the cursor, alpha is dropped down to 20%.
    float alpha_adjustment = 1.0;
    if (in_block_coordinates.z > cursor_location.z)
    {
      alpha_adjustment = 0.2 - clamp((in_block_coordinates.z -
                                      cursor_location.z) / 100, 0, 0.2);
    }

    // Diffuse color passed to the fragment shader.
    pass_color_diffuse = vec4(in_color.r + gnomon_color.r + cursor_color.r,
                              in_color.g + gnomon_color.g + cursor_color.g,
                              in_color.b + gnomon_color.b + cursor_color.b,
                              in_color.a*alpha_adjustment);

    // Specular color passed to the fragment shader.
    pass_color_specular = in_color_specular;
}
