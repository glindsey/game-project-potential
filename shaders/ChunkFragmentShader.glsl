#version 330 core

in vec4 pass_color_diffuse;
in vec4 pass_color_specular;
in vec3 pass_position_worldspace;
in vec3 pass_normal_cameraspace;
in vec3 pass_eye_direction_cameraspace;
in vec3 pass_light_direction_cameraspace;

out vec4 out_color;

uniform vec3 light_color;
uniform float light_power;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

uniform vec3 light_position_worldspace;

void main()
{
  vec3 color_material_diffuse = vec3(0.9, 0.9, 0.9) * pass_color_diffuse.rgb;
  vec3 color_material_ambient = vec3(0.1, 0.1, 0.1) * pass_color_diffuse.rgb;
  vec3 color_material_specular = pass_color_specular.rgb;

  // Distance from the light to the vertex
  float distance = length(light_position_worldspace - pass_position_worldspace);

  // Normal of the computed fragment in camera space.
  vec3 n = normalize(pass_normal_cameraspace);

  // Direction of the light, in camera space.
  vec3 l = normalize(pass_light_direction_cameraspace);

  // Cosine of the angle between the normal and light direction, clamped
  // between 0 and 1.
  float cos_theta = clamp(dot(n, l), 0, 1);

	// Eye vector (towards the camera)
	vec3 e = normalize(pass_eye_direction_cameraspace);

	// Direction in which the triangle reflects the light
	vec3 r = reflect(-l, n);

	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cos_alpha = clamp(dot(e, r), 0, 1);

  // Create the proper color.
	out_color.rgb =
		// Ambient : simulates indirect lighting
		color_material_ambient +
		// Diffuse : "color" of the object
		(color_material_diffuse * light_color * light_power * cos_theta / (distance * distance)) +
		// Specular : reflective highlight, like a mirror
		(color_material_specular * light_color * light_power * pow(cos_alpha, 5) / (distance * distance));

  // Create the alpha channel.
  out_color.a = pass_color_diffuse.a + (pass_color_specular.a * pow(cos_alpha, 5) / (distance * distance));
}
