game-project-potential
======================

A 3D voxel-based attempt to do something similar to Dwarf Fortress.

This project is largely incomplete, and is basically my first foray into straight OpenGL 3.0 and GLSL coding.

It is built in Code::Blocks, using MinGW GCC, and expects the following libraries: SOIL, SFML, Boost, GLEW, and libnoise.  Boost must be compiled, as the code utilizes the chrono and filesystem modules.  As with any C++ libraries, they should be built with the same build of MinGW GCC used to build the application.

SFML is used only for window creation and event handling; my goal is to eventually get rid of it entirely and replace it with something like GLFW.

UI:
  Use the arrow keys to move the cursor on the current (X/Z) plane.
  Use the comma and period keys to move the cursor up/down on the Y axis.
  Use CTRL+arrow keys to tilt the camera.
  Use CTRL+comma and CTRL+period to zoom the camera in and out.
  Use ESC to exit.

Known issues:
  * There are obviously no textures of any kind at the moment.
  * It can be a bit hard to determine exactly where on the stage the cursor is; I'm trying to figure out a way to make this clearer, graphically.
  * Light calculations in the shader aren't right, causing lighting to look wrong, particularly if you tilt the camera up to look straight down.  Probably an error in the vector calculations somewhere.
  * The "determine known blocks" algorithm isn't quite right and results in some bricks being unknown when they shouldn't be.
  * Unknown blocks are currently not rendered, instead of being rendered as an "unknown" brick type.  Future code will render them as a TV-static sort of grey fuzz.
  * The "peephole" to peer through terrain to the cursor location isn't quite correct; the cursor might still be obscured.
