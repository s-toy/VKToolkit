#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 _inVertexPosition;
layout(location = 1) in vec3 _inVertexNormal;
layout(location = 2) in vec2 _inVertexTexCoord;

layout(location = 0) out vec3 _outPositionW;
layout(location = 1) out vec3 _outNormalW;
layout(location = 2) out vec2 _outTexCoord;

layout(set = 0, binding = 0) uniform UBO{
	mat4 Model;
	mat4 View;
	mat4 Projection;
} Ubo;

void main()
{
	_outPositionW = vec3(Ubo.Model * vec4(_inVertexPosition, 1.0));
	_outNormalW = mat3(transpose(inverse(Ubo.Model))) * _inVertexNormal;
	_outTexCoord = _inVertexTexCoord;
	gl_Position = Ubo.Projection * Ubo.View * vec4(_outPositionW, 1.0);
}