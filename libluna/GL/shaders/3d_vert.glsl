#version 330 core

#include "common3d.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 vFragPos;
out vec2 vTexCoord;
out vec3 vNormal;
out mat3 vTBN;

void main()
{
  mat4 t = uTransform.projection * uTransform.view * uTransform.model;
  gl_Position = t * vec4(aPos, 1.0);
  // fragPos = vec3(model * vec4(aPos, 1.0);
  vFragPos = aPos;
  vTexCoord = aTexCoord;
  vNormal = mat3(transpose(inverse(uTransform.model))) * aNormal;

  vec3 T = normalize(vec3(uTransform.model * vec4(aTangent, 0.0)));
  vec3 B = normalize(vec3(uTransform.model * vec4(aBitangent, 0.0)));
  vec3 N = normalize(vec3(uTransform.model * vec4(aNormal, 0.0)));
  vTBN = transpose(mat3(T, B, N));
}
