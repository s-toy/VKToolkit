#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;//这个location与顶点着色器中的location相关联
layout(location = 0) out vec4 outColor;//location = 0,代表的是帧缓冲的索引，颜色会被连接到索引为0的帧缓冲上
//如果改成其它的数如1,2的话，就没有图像输出


//main函数会被每个片段调用
void main()
{
	outColor = vec4(fragColor,1.0);//fragColor的值会自动插值到三个顶点之间的片段中
}
