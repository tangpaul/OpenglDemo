#shader vertex
#version 330 core
layout (location = 0) in vec4 aPos;
layout(location = 1) in vec2 texPos;

out vec2 texCorrd;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * aPos;
	texCorrd = texPos;
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 texCorrd;

uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture2D(u_Texture, texCorrd);
	FragColor = texColor;
};