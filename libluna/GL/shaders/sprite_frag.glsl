#version 330 core

in vec2 vTexCoord;

uniform sampler2D uSpriteTexture;

out vec4 fColor;

void main()
{
  fColor = texture(uSpriteTexture, vTexCoord);
}
