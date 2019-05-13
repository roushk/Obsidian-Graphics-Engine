#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include "BoundingVolume.h"


struct Object;

class BoundingSphereCentroid : public BoundingVolume
{
public:
  BoundingSphereCentroid(Object& obj);
  BoundingSphereCentroid(const std::vector< std::pair<Object*, glm::vec3>>& offsets);
  void Update(float dt, glm::mat4 transform = glm::mat4(1.0f)) override {};
  void Update(float dt, glm::vec3 offset);
  
  glm::vec3 center;
  glm::vec3 min, max;
  float radius;
  Object& obj;

};
