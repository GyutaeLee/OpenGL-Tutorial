#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 WorldPos;

void main()
{
	WorldPos = aPos;

	mat4 rotview = mat4(mat3(view));
	vec4 clipPos = projection * rotview * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}