#version 440 core

in vec3 texturePos;

out vec4 color;

uniform samplerCube skyboxTexture;

void main()
{
	color = texture(skyboxTexture, texturePos).bgra;
}