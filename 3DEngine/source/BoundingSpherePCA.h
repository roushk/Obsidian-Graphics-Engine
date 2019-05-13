#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include "BoundingSphereCentroid.h"


struct Object;

class BoundingSpherePCA
{
public:
  BoundingSpherePCA(Object& obj);
  
  void Update(float dt, glm::mat4 transform = glm::mat4(1.0f));
  void Update(float dt, glm::vec3 offset);
  glm::mat3 CovarianceMatrix();
  
  void EigenSphere();
  void Jacobi(glm::mat3& a, glm::mat3& v);
  void SymSchur(glm::mat3& a, int p, int q, float &c, float &s);
  void ExtremePointsAlongDirection(glm::vec3 dir, int* imin, int *imax);
  
  Object& obj;

  glm::vec3 oldOffset{ 0,0,0 };
  glm::vec3 center{ 0,0,0 };
  glm::vec3 min, max;
  float radius;

  std::vector<glm::vec3> verts;
  std::vector<glm::uvec2> faces;
};
