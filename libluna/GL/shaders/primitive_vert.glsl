#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec2 uScreenSize;
uniform vec2 uPrimitivePos;

void main()
{
  vec2 translatedPos = aPos + uPrimitivePos;
  gl_Position = vec4((-1 + (translatedPos / uScreenSize) * 2), 0.0, 1.0);
}
