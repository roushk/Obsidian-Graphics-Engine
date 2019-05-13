#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include "BoundingSphereCentroid.h"


struct Object;

class BoundingSphereRitter
{
public:
  BoundingSphereRitter(Object& obj);
  BoundingSphereRitter(const BoundingSphereRitter& rhs);
  BoundingSphereRitter& operator=(const BoundingSphereRitter& rhs);
  void Update(float dt, glm::mat4 transform = glm::mat4(1.0f));
  void Update(float dt, glm::vec3 offset);

  void MostSeparatedPoints(int& min, int& max);
  void SphereFromDistPoints();
  void SphereOfSphereAndPt(glm::vec3& point);

  glm::vec3 center{0,0,0};
  glm::vec3 min, max;
  float radius;

  std::vector<glm::vec3> verts;
  std::vector<glm::uvec2> faces;
  glm::vec3 oldOffset{ 0,0,0 };

  Object& obj;
  


};
