#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_FragPosition;
layout(location = 1) in vec2 in_TexCoord;
layout(location = 2) in vec3 in_Normal;

layout(location = 0) out vec4 out_Position;
layout(location = 1) out vec4 out_Normal;
layout(location = 2) out vec4 out_Color;

layout(set = 1, binding = 0) uniform sampler2D Sampler_Diff;
layout(set = 1, binding = 1) uniform sampler2D Sampler_Spec;

void main()
{
	out_Position = vec4(in_FragPosition, 1.0f);

	out_Normal = vec4(normalize(in_Normal), 1.0f);

	out_Color.rgb = texture(Sampler_Diff, in_TexCoord).rgb;
	out_Color.a = texture(Sampler_Spec, in_TexCoord).r;
}