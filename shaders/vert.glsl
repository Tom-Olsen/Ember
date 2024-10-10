#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 0) out vec4 outVertexColor;

void main()
{
	gl_Position = vec4(inPosition, 1.0);
	outVertexColor = inColor;
}