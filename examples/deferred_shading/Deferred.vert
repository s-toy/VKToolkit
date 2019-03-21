#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_TexCoord;

layout(location = 0) out vec2 out_TexCoord;

void main()
{
	out_TexCoord = in_TexCoord;

	gl_Position = vec4(in_Position, 1.0f);
}