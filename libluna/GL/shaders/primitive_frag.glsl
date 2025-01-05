#version 330 core

uniform vec4 uPrimitiveColor;

out vec4 fColor;

void main()
{
  fColor = uPrimitiveColor;
}
