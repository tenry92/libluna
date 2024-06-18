#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 uScreenSize;
uniform vec2 uSpritePos;

out vec2 vTexCoord;

void main()
{
  vec2 translatedPos = aPos + uSpritePos;
  gl_Position = vec4((-1 + (translatedPos / uScreenSize) * 2), 0.0, 1.0);
  vTexCoord = aTexCoord;
}
