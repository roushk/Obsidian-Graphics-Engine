/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: GJK.h
Purpose: gjk header
Project: coleman.jonas_CS350_4
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 4/19/19
End Header --------------------------------------------------------*/

#pragma once
#include <glm/detail/type_vec3.hpp>
#include <vector>

struct BoundingVolumeHeiarchyNode;
struct Sphere;
class AxisAlignedBoundingBox;
struct Object;
struct OctreeNode;
struct BSPNode;

class BoundingVolumeHeiarchy_AABB;

bool AABB_to_SphereAABB(const AxisAlignedBoundingBox& obj1, const Sphere& obj2);
bool Octree_to_SphereAABB(const OctreeNode* obj1, const Sphere& obj2);

struct SimplexPoint
{
  glm::vec3 p;
  float bary;
};

glm::vec2 LineBary(glm::vec3& a, glm::vec3& b);
glm::vec3 TriBary(glm::vec3& a, glm::vec3& b, glm::vec3& c);
glm::vec4 TetBary(glm::vec3& a, glm::vec3& b, glm::vec3& c, glm::vec3& d);


struct Simplex
{
  SimplexPoint a;
  SimplexPoint b;
  SimplexPoint c;
  SimplexPoint d;


  void AddPoint(glm::vec3& point);
  
  void Line();
  void Tri();
  void Tet();

  glm::vec3 getDirection();
  glm::vec3 BaryPoint();
  
  int count = 0;

};

class GJK
{
public:

  bool checkCollision_broad(const Sphere& obj, BoundingVolumeHeiarchy_AABB &bvh);
  bool checkCollision_broad(const Sphere& obj, OctreeNode *node);
  bool checkCollision_broad(const Sphere& obj, BSPNode *node);

  bool checkCollision_mid(const Sphere& obj, BoundingVolumeHeiarchyNode* bvh);
  bool checkCollision_mid(const Sphere& obj, OctreeNode *node);
  bool checkCollision_mid(const Sphere& obj, BSPNode *node);

  bool checkCollision_narrow(const Sphere& obj, BoundingVolumeHeiarchyNode* bvh);
  bool checkCollision_narrow(const Sphere& obj, OctreeNode *node);
  bool checkCollision_narrow(const Sphere& obj, BSPNode *node);

  bool checkCollision_GJK(const Sphere& obj, const std::vector<glm::vec3> & pointCloud);


};
