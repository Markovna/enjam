#version 410 core

layout (location = 0) in vec3 aPos;

layout (std140) uniform perView
{
   mat4 projection;
   mat4 view;
};

void main()
{
   gl_Position = projection * view * vec4(aPos, 1.0);
}