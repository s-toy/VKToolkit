#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_FragColor;
layout(location = 1) in vec2 in_TexCoord;

layout(location = 0) out vec4 out_FragColor;

layout(binding = 1) uniform sampler2D TexSampler;

void main()
{
	out_FragColor = vec4(in_FragColor * texture(TexSampler, in_TexCoord).rgb, 1.0);
}