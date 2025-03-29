#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_texCoord;
uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

out vec2 TexCoord;
void main()
{
	gl_Position = u_ViewProjection *u_Model* vec4(a_Position, 1.0f);
	TexCoord=a_texCoord;
}