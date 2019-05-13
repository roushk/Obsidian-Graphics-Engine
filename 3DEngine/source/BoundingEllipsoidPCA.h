#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


struct Object;

class BoundingEllipsoidPCA
{
public:
  BoundingEllipsoidPCA(Object& obj);
  void Update(float dt, glm::mat4 transform = glm::mat4(1.0f));
  glm::vec3 center;
  glm::vec3 oldOffset;
  float radius;
};
