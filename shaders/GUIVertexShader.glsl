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
layout (location = 2) in vec2 in_textureUV;
layout (location = 3) in float in_textured;

out vec4 pass_color;
out vec2 pass_textureUV;
out float pass_textured;

uniform vec2 window_size;
uniform uint frame_counter;

void main()
{
    // Get the half-window-sizes.
    vec2 half_window_size = vec2(window_size.x / 2, window_size.y / 2);

    // Vertex position is super-simple:
    //  translate (0.0) - (window_size.x, window_size.y)
    //         to (-1, -1) - (1, 1), and invert y (because OpenGL goes from
    //         bottom-to-top instead of top-to-bottom).
    gl_Position = vec4((in_position.x / half_window_size.x) - 1,
                       1 - (in_position.y / half_window_size.y),
                       0.0f, 1.0f);

    // Pass the texture sample and the color.
    // TODO: Later allow for pulsing stuff, etc.
    pass_color = in_color;
    pass_textureUV = in_textureUV;
    pass_textured = in_textured;
}
