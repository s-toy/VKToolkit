#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;//���location�붥����ɫ���е�location�����
layout(location = 0) out vec4 outColor;//location = 0,�������֡�������������ɫ�ᱻ���ӵ�����Ϊ0��֡������
//����ĳ�����������1,2�Ļ�����û��ͼ�����


//main�����ᱻÿ��Ƭ�ε���
void main()
{
	outColor = vec4(fragColor,1.0);//fragColor��ֵ���Զ���ֵ����������֮���Ƭ����
}
