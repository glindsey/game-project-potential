#version 330 core

// Cursor vertex shader: Used to show a wireframe cursor.

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

layout (location = 0) in vec3 in_position_modelspace;
layout (location = 1) in vec4 in_color;

out vec4 pass_color;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

uniform uint frame_counter;

const vec3 zero_vector = vec3(0, 0, 0);

void main()
{
    // Output position of the vertex, in clip space: MVP * position
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(in_position_modelspace, 1.0);

    // Get modulo of frame counter to do pulsing features.
    // (Is modulo faster, or would a bitwise AND be better?)
    uint frame_counter_modulo = frame_counter % 256u;

    // Color passed to the fragment shader.
    pass_color = in_color;
}
