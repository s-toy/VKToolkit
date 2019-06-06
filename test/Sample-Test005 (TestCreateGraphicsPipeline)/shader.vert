#version 450

//三角形的顶点坐标
vec2 positions[3]= vec2[](
	vec2(0.0,-0.5),
	vec2(0.5,0.5),
	vec2(-0.5,0.5)
);
//三角形的顶点的颜色数组
vec3 colors[3] = vec3[](
	vec3(1.0,0.0,0.0),
	vec3(0.0,1.0,0.0),
	vec3(0.0,0.0,1.0)
);

layout(location = 0) out vec3 fragColor;//这个location与片段着色器中拥有相同的location的变量相关联
//main函数会被每个顶点调用
void main()
{
	//gl_VertexIndex是顶点的索引，通常是顶点缓冲的索引，但是在这里是顶点数组的索引
	gl_Position = vec4(positions[gl_VertexIndex],0.0,1.0);
	fragColor = colors[gl_VertexIndex];
}

//着色器的取名的结尾必须是阶段，如何vert，frag等等