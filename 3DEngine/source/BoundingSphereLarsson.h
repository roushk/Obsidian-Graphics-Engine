#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>


struct Sphere;
struct Object;



struct ExtremalSet
{
  glm::vec3 min;
  glm::vec3 max;
};

enum class EPOS
{
  EPOS6,
  EPOS14,
  EPOS26,
  EPOS98
};
const std::vector<glm::vec3> EPOS6
{ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }};

const std::vector<glm::vec3> EPOS14
{ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 },{ 1, 1, 1 }, 
  { 1, 1, -1 }, { 1, -1, 1 }, { 1, -1, -1 } };

const std::vector<glm::vec3> EPOS26
{ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 },{ 1, 1, 1 }, 
  { 1, 1, -1 }, { 1, -1, 1 }, { 1, -1, -1 },{ 1, 1, 0 }, 
  { 1, -1, 0 }, { 1, 0, 1 }, { 1, 0, -1 }, { 0, 1, 1 }, { 0, 1, -1 } };

const std::vector<glm::vec3> EPOS98
{ { 1, 0,   0 }, { 0, 1,  0 }, { 0, 0, 1 },{ 1, 1, 1 },
  { 1, 1,  -1 }, { 1, -1, 1 }, { 1, -1, -1 },{ 1, 1, 0 },
  { 1, -1,  0 }, { 1, 0,  1 }, { 1, 0, -1 }, { 0, 1, 1 }, { 0, 1, -1 },
  { 1, 2,   0 }, { 1, -2, 0 }, { 1, 0, 2 }, { 1, 0, -2 }, { 0, 1, 2 }, 
  { 0, 1,  -2 }, { 2, 1,  0 }, { 2, -1, 0 }, { 2, 0, 1 }, { 2, 0, -1 }, 
  { 0, 2,   1 }, { 0, 2,  -1 }, { 1, 1, 2 }, { 1, 2, 1 }, { 2, 1, 1 }, 
  { 1, -1,  2 }, { 1, -2,  1 }, { 2, -1, 1 }, { 1, 1, -2 }, { 1, 2, -1 }, 
  { 2, 1,  -1 }, { 1, -1, -2 }, { 1, -2, -1 }, { 2, -1, -1 } ,{ 1, 2, 2 },
  { 2, 1,   2 }, { 2, 2,   1 }, { 1, -2, 2 }, { 2, -1, 2 }, { 2, -2, 1 }, 
  { 1, 2,  -2 }, { 2, 1, - 2 }, { 2, 2, -1 }, { 1, -2, -2 }, { 2, -1, -2 }, 
  { 2, -2, -1 } };

class BoundingSpherelarsson
{
public:
  BoundingSpherelarsson(Object& obj);
  void Update(float dt, glm::mat4 transform = glm::mat4(1.0f));
  void Update(float dt, glm::vec3 offset);

  void SphereOfSphereAndPt(Sphere& sphere, glm::vec3& point);

  Sphere Gartner(std::vector<glm::vec3> extremalPoints);
  std::vector<glm::vec3> FindExtremalPoints();
  Sphere MinimumSphere(std::vector<glm::vec3>& E);

  std::vector<glm::vec3> verts;
  std::vector<glm::vec3> normals;
  std::vector<glm::uvec2> faces;
  
  glm::vec3 min, max;
  glm::vec3 center;
  glm::vec3 oldOffset{ 0,0,0 };
  
  Object& obj;
  EPOS currentEPOS = EPOS::EPOS6;
  float radius;
};
