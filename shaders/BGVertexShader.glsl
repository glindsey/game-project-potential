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

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec4 in_color;

out vec4 pass_color;

uniform uint frame_counter;

void main()
{
    gl_Position = vec4(in_position.x, -in_position.y, 1.0f, 1.0f);
    pass_color = in_color;
}
