#version 450 core

layout (location = 0) in vec3 INposition;
layout (location = 1) in vec4 INcolor;
layout (location = 0) out vec4 OUTvertexColor;

void main()
{
	gl_Position = vec4(INposition, 1.0);
	OUTvertexColor = INcolor;
}