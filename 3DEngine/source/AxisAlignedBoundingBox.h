#pragma once
#include <glm/vec3.hpp>
#include "BoundingVolume.h"
#include <vector>
#include <glm/mat4x4.hpp>


struct Object;

class AxisAlignedBoundingBox : public BoundingVolume
{
public:
  AxisAlignedBoundingBox(Object& obj);
  AxisAlignedBoundingBox(const std::vector < std::pair<Object*, glm::vec3>>& offsets);
  void Update(float dt, glm::mat4 transform) override {};
  void Update(float dt, glm::vec3 offset);

  glm::vec3 min, max;
  Object& obj;


};
