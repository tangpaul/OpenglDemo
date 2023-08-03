#shader vertex
#version 330 core
layout (location = 0) in vec4 aPos;
layout(location = 1) in vec4 aColor;

out vec4 ourColor;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * aPos;
	ourColor = aColor;
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec4 ourColor;

void main()
{
	FragColor = ourColor;
};