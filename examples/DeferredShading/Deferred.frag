#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 in_TexCoord;

layout(location = 0) out vec4 out_FragColor;

layout(binding = 0) uniform UBO{
	vec3 ViewPosition;
} Ubo;

layout(binding = 1) uniform sampler2D Sampler_Position;
layout(binding = 2) uniform sampler2D Sampler_Normal;
layout(binding = 3) uniform sampler2D Sampler_Color;

struct Light
{
    vec3 Position;
    vec3 Color;
};

const int NUM_LIGHT = 7;

const Light lights[NUM_LIGHT]={
	{{0.0f, 3.0f, 1.0f},{1.5f,1.5f,1.5f}},
	{{2.0f, 0.0f, 3.0f},{1.0f,0.0f,0.0f}},
	{{2.0f, 1.0f, 3.0f},{0.0f, 0.0f, 2.5f}},
	{{0.0f, 4.9f, 3.5f},{1.0f, 1.0f, 0.0f}},
	{{0.0f, 0.5f, 3.0f},{0.0f, 1.0f, 0.2f}},
	{{0.0f, 2.0f, 4.0f},{1.0f, 0.7f, 0.3f}},
	{{0.7f, 1.0f, 2.0f},{0.7f, 0.6f, 0.5f}}
};

const float Linear = 0.7;
const float Quadratic = 1.8;

void main()
{             
    vec3 FragPosition = texture(Sampler_Position, in_TexCoord).rgb;
    vec3 Normal = texture(Sampler_Normal, in_TexCoord).rgb;
    vec3 Diffuse = texture(Sampler_Color, in_TexCoord).rgb;
    float Specular = texture(Sampler_Color, in_TexCoord).a;
    
    vec3 Lighting  = Diffuse * 0.1; 
    vec3 ViewDir  = normalize(Ubo.ViewPosition - FragPosition);
    for(int i = 0; i < NUM_LIGHT; ++i)
    {
        vec3 LightDir = normalize(lights[i].Position - FragPosition);
        vec3 Diffuse = max(dot(Normal, LightDir), 0.0) * Diffuse * lights[i].Color;

        vec3 HalfwayDir = normalize(LightDir + ViewDir);  
        float Spec = pow(max(dot(Normal, HalfwayDir), 0.0), 32.0);
        vec3 S = lights[i].Color * Spec * Specular;

        float Distance = length(lights[i].Position - FragPosition);
        float Attenuation = 1.0 / (1.0 + Linear * Distance + Quadratic * Distance * Distance);
        Diffuse *= Attenuation;
        S *= Attenuation;
        Lighting += Diffuse + S; 

        Lighting += Diffuse + S;        
    }
    out_FragColor = vec4(Lighting, 1.0);
}
