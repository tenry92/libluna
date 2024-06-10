#version 330 core

#include "common3d.glsl"

in vec3 vFragPos;
in vec2 vTexCoord;
in vec3 vNormal;
in mat3 vTBN;

out vec4 fColor;

void main()
{
  vec4 diffuse = texture(uMaterial.diffuseMap, vTexCoord);

  //
  // vec3 normal = texture(material.normalMap, texCoord).rgb; // [0,1]
  // normal = normalize(normal * 2.0 - 1.0); // [-1,1]
  //

  vec3 norm = normalize(vNormal);
  vec3 lightDir = normalize(uPointLights[0].position - vFragPos);
  float diff = max(dot(norm, lightDir), 0.0);

  vec3 ambient = uAmbientLight.color * uAmbientLight.intensity;
  vec3 point = uPointLights[0].color * diff;
  
  fColor = vec4(ambient + point, 1.0) * diffuse;
}
