#version 440

//input vertex data
layout(location = 0) in vec3 vertex_position_modelspace;
layout(location = 1) in vec2 vertex_uv;

// Output data
out vec2 uv;

void main()
{

    gl_Position.xyz = vertex_position_modelspace;
    gl_Position.w = 1.0;
	uv = vertex_uv;
}