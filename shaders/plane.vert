#version 400 core
layout (location = 0) in vec3 position;

out vec3 texCoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

uniform float step;

// const vec3 texcoords[4] = vec3[4](
// vec3(0.0, 1.0, 0),
// vec3(0.0, 0.0, 0),
// vec3(1.0, 1.0, 0),
// vec3(1.0, 0.0, 0)
// );

void main(void)
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	// texCoord = texcoords[gl_InstanceID % 4];
	texCoord = vec3(position.x * step, position.z * step, 0);
}
