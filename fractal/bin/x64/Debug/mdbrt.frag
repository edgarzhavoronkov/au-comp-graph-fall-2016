﻿#version 440 core

uniform float scale;
uniform vec2 offset;
uniform int max_iter;
uniform sampler1D texture_sampler;
in vec2 uv;
out vec3 color;

void main()
{
	vec2 c = vec2(uv[0] * scale + offset[0], uv[1] * scale + offset[1]);
    vec2 z = c;

    int i = 0;
    for(; i < max_iter && ((z[0] * z[0] + z[1] * z[1]) < 4.0); i++)
	{
        z = vec2(z.x * z.x - z.y * z.y + c.x, z.y * z.x + z.x * z.y + c.y);
    };
	color = texture(texture_sampler, float(i) / float(max_iter)).rgb;
	if (i == max_iter)
	{
		color = vec3(0);
	}
}