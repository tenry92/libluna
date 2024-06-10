#define POINT_LIGHT_COUNT 1

struct Transform {
  mat4 model;
  mat4 view;
  mat4 projection;
};

struct AmbientLight {
  vec3 color;
  float intensity;
};

struct PointLight {
  bool enabled;
  vec3 color;
  vec3 position;
};

struct Material {
  sampler2D diffuseMap;
  sampler2D normalMap;
};

uniform Material uMaterial;
uniform AmbientLight uAmbientLight;
uniform PointLight[POINT_LIGHT_COUNT] uPointLights;
uniform vec3 uViewPos;

uniform Transform uTransform;
