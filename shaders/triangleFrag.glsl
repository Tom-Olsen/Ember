#version 450 core

layout(location = 0) in vec4 INcolor;
layout(location = 0) out vec4 OUTcolor;

void main()
{
	OUTcolor = INcolor;
}