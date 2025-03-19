#version 400 core
layout (location = 0) out vec4 fragColor;

in vec3 texCoord;
uniform sampler2DArray tex;

void main(void)
{
	// fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	fragColor = texture(tex, texCoord).rgba;
}
