#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct Sphere
{
  Sphere(const glm::vec3& center, float radius) : center(center), radius(radius) {}
  Sphere(const glm::vec4& center, float radius) : center(glm::vec3( center.x, center.y, center.z)), radius(radius) {}

  glm::vec3 center;
  float radius;
};

struct Object;

class BoundingVolume
{
public:
  BoundingVolume(Object& obj);
  virtual void Update(float dt, glm::mat4 transform = glm::mat4(1.0f)){};
  
  glm::vec3 center;
};
