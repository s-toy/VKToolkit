#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
layout(location = 2) in vec2 in_TexCoord;

layout(location = 0) out vec3 out_FragColor;
layout(location = 1) out vec2 out_TexCoord;

layout(binding = 0) uniform UBO{
	mat4 Model;
	mat4 View;
	mat4 Projection;
} Ubo;

void main()
{
	gl_Position = Ubo.Projection * Ubo.View * Ubo.Model * vec4(in_Position, 1.0);
	out_FragColor = in_Color;
	out_TexCoord = in_TexCoord;
}
