#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_TexCoord;
layout(location = 2) in vec3 in_Normal;

layout(location = 0) out vec3 out_FragPosition;
layout(location = 1) out vec2 out_TexCoord;
layout(location = 2) out vec3 out_Normal;

layout(set = 0, binding = 0) uniform UBO{
	mat4 Model;
	mat4 View;
	mat4 Projection;
} Ubo;

void main()
{
	vec4 WorldPosition = Ubo.Model * vec4(in_Position, 1.0f);
	out_FragPosition = vec3(WorldPosition);

	out_TexCoord = in_TexCoord;

	mat3 NormalMatrix = transpose(inverse(mat3(Ubo.Model)));
	out_Normal = NormalMatrix * in_Normal;

	gl_Position = Ubo.Projection * Ubo.View * WorldPosition;
}
