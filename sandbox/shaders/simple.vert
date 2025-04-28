#version 410 core

struct ObjectUniform {
  mat4 model;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (std140) uniform perView {
   mat4 projection;
   mat4 view;
};

layout (std140) uniform perObject {
   ObjectUniform data;
};

out vec2 TexCoord;

void main()
{
   mat4 model = data.model;
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   TexCoord = aTexCoord;
}