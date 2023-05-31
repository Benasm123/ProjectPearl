#version 450 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_tex;

layout (push_constant) uniform constants {
	mat4 m;
	mat4 mvp;
} pushConstant;	

void main(void) 
{
	gl_Position = pushConstant.mvp * vec4(i_position, 1.0f);
}