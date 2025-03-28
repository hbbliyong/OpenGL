#version 450 core

layout (location = 0) out vec4 o_Color;

in vec2 TexCoord;
uniform vec4 u_Color;
uniform sampler2D u_Texture;
void main()
{
	//o_Color = u_Color+texture(u_Texture,TexCoord);
	o_Color =texture(u_Texture,TexCoord);
}