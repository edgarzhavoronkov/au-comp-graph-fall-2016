#version 440 core

in vec3 normal;

out vec4 color;

uniform samplerCube skyboxTexture;
uniform vec3 fromCamera;

void main()
{
	vec3 texturePos = reflect(fromCamera, normalize(normal));
	color = texture(skyboxTexture, texturePos).bgra;
}
